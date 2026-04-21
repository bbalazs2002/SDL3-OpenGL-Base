#include "GLUtils.hpp"

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <SDL3_image/SDL_image.h>

#ifndef PROJECT_ROOT
#pragma message(__FILE__ "("  ": warning: PROJECT_ROOT is not defined! Fallback route: ./")
#define PROJECT_ROOT "./"
#endif

static void loadShaderCode( std::string& shaderCode, const std::filesystem::path& fileName )
{
	const std::filesystem::path fullPath = std::filesystem::path(PROJECT_ROOT) / fileName;

	// shaderkod betoltese _fileName fajlbol
	shaderCode = "";

	// _fileName megnyitasa
	std::ifstream shaderStream(fullPath);
	if ( !shaderStream.is_open() )
	{
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR,
						SDL_LOG_PRIORITY_ERROR,
						"Error while opening shader code file %s!", fullPath.string().c_str());
		return;
	}

	// file tartalmanak betoltese a shaderCode string-be
	std::string line = "";
	while ( std::getline( shaderStream, line ) )
	{
		shaderCode += line + "\n";
	}

	shaderStream.close();
}

GLuint AttachShader( const GLuint programID, GLenum shaderType, const std::filesystem::path& fileName )
{
	const std::filesystem::path fullPath = std::filesystem::path(PROJECT_ROOT) / fileName;

    // shaderkod betoltese _fileName fajlbol
    std::string shaderCode;
    loadShaderCode( shaderCode, fullPath);

    return AttachShaderCode( programID, shaderType, shaderCode );
}

GLuint AttachShaderCode( const GLuint programID, GLenum shaderType, std::string_view shaderCode )
{
	if (programID == 0)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_ERROR,
						SDL_LOG_PRIORITY_ERROR,
						"Program needs to be inited before loading!");
		return 0;
	}

	// shader letrehozasa
	GLuint shaderID = glCreateShader( shaderType );

	// kod hozzarendelese a shader-hez
	const char* sourcePointer = shaderCode.data();
	GLint sourceLength = static_cast<GLint>( shaderCode.length() );

	glShaderSource( shaderID, 1, &sourcePointer, &sourceLength );

	// shader leforditasa
	glCompileShader( shaderID );

	// ellenorizzuk, h minden rendben van-e
	GLint result = GL_FALSE;
	int infoLogLength;

	// forditas statuszanak lekerdezese
	glGetShaderiv( shaderID, GL_COMPILE_STATUS, &result );
	glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );

	if ( GL_FALSE == result || infoLogLength != 0 )
	{
		// hibauzenet elkerese es kiirasa
		std::string ErrorMessage( infoLogLength, '\0' );
		glGetShaderInfoLog( shaderID, infoLogLength, NULL, ErrorMessage.data() );

		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR,
						( result ) ? SDL_LOG_PRIORITY_WARN : SDL_LOG_PRIORITY_ERROR,
						"[glCompileShader]: %s", ErrorMessage.data() );
	}

	// shader hozzarendelese a programhoz
	glAttachShader( programID, shaderID );

	return shaderID;

}

void LinkProgram( const GLuint programID, bool OwnShaders )
{
	// illesszük össze a shadereket (kimenő-bemenő változók összerendelése stb.)
	glLinkProgram( programID );

	// linkeles ellenorzese
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv( programID, GL_LINK_STATUS, &result );
	glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );
	if ( GL_FALSE == result || infoLogLength != 0 )
	{
		std::string ErrorMessage( infoLogLength, '\0' );
		glGetProgramInfoLog( programID, infoLogLength, nullptr, ErrorMessage.data() );
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR,
						( result ) ? SDL_LOG_PRIORITY_WARN : SDL_LOG_PRIORITY_ERROR,
						"[glLinkProgram]: %s", ErrorMessage.data() );
	}

	// Ebben az esetben a program objektumhoz tartozik a shader objektum.
	// Vagyis a shader objektumokat ki tudjuk "törölni".
    // Szabvány szerint (https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml)
    // a shader objektumok csak akkor törlődnek, ha nincsennek hozzárendelve egyetlen program objektumhoz sem.
	// Vagyis mikor a program objektumot töröljük, akkor törlődnek a shader objektumok is.
	if ( OwnShaders )
	{
		// kerjuk le a program objektumhoz tartozó shader objektumokat, ...
        GLint attachedShaders = 0;
        glGetProgramiv( programID, GL_ATTACHED_SHADERS, &attachedShaders );
        std::vector<GLuint> shaders( attachedShaders );

        glGetAttachedShaders( programID, attachedShaders, nullptr, shaders.data() );

        // ... es "toroljuk" oket
        for ( GLuint shader : shaders )
        {
            glDeleteShader( shader );
        }

	}
}

static inline ImageRGBA::TexelRGBA* get_image_row( ImageRGBA& image, int rowIndex )
{
	return &image.texelData[  rowIndex * image.width ];
}

static void invert_image_RGBA(ImageRGBA& image)
{
	int height_div_2 = image.height / 2;


	for ( int index = 0; index < height_div_2; index++ )
	{
		std::uint32_t* lower_data  =reinterpret_cast<std::uint32_t*>(get_image_row( image, index) );
		std::uint32_t* higher_data =reinterpret_cast<std::uint32_t*>(get_image_row( image, image.height - 1 - index ) );

		for ( unsigned int rowIndex = 0; rowIndex < image.width; rowIndex++ )
		{
			lower_data[ rowIndex ] ^= higher_data[ rowIndex ];
			higher_data[ rowIndex ] ^= lower_data[ rowIndex ];
			lower_data[ rowIndex ] ^= higher_data[ rowIndex ];
		}
	}
}

GLsizei NumberOfMIPLevels( const ImageRGBA& image )
{
	GLsizei targetlevel = 1;
	unsigned int index = std::max( image.width, image.height );

	while (index >>= 1) ++targetlevel;

	return targetlevel;
}

[[nodiscard]]
ImageRGBA ImageFromFile(const std::filesystem::path& fileName, bool needsFlip) {
	const std::filesystem::path fullPath = std::filesystem::path(PROJECT_ROOT) / fileName;

	ImageRGBA img;

	// 1. Fájl megnyitása UTF-8 útvonallal
	std::string pathStr = fullPath.u8string();
	SDL_IOStream* stream = SDL_IOFromFile(pathStr.c_str(), "rb");

	if (!stream) {
		SDL_Log("File not found: %s", pathStr.c_str());
		return img;
	}

	// 2. Betöltés - Az SDL3-image automatikusan felismeri a PNG/JPG stb. formátumot.
	// A második paraméter 'true', így az IMG_Load_IO lezárja a stream-et helyettünk.
	SDL_Surface* raw_surf = IMG_Load_IO(stream, true);

	if (!raw_surf) {
		// Ha itt "Unsupported format" hibát kapsz, akkor a vcpkg build-nél maradt ki a feature.
		SDL_Log("IMG_Load error (%s): %s", pathStr.c_str(), SDL_GetError());
		return img;
	}

	// 3. Smart pointer a biztonságos kezeléshez (SDL3-ban DestroySurface)
	std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> loaded_img(raw_surf, SDL_DestroySurface);

	// 4. Konvertálás a kívánt formátumra (SDL3: SDL_ConvertSurface)
	// Megjegyzés: Az SDL_PIXELFORMAT_RGBA32 egy "alias", ami mindig a jó bájtsorrendet választja.
	SDL_Surface* formatted_raw = SDL_ConvertSurface(loaded_img.get(), SDL_PIXELFORMAT_RGBA32);

	if (!formatted_raw) {
		SDL_Log("Conversion error: %s", SDL_GetError());
		return img;
	}

	std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> formattedSurf(formatted_raw, SDL_DestroySurface);

	// 5. Adatok áthelyezése a saját osztályodba
	img.Assign(reinterpret_cast<const std::uint32_t*>(formattedSurf->pixels),
		formattedSurf->w,
		formattedSurf->h);

	if (needsFlip) {
		invert_image_RGBA(img);
	}

	return img;
}

void CleanOGLObject( OGLObject& ObjectGPU )
{
	glDeleteBuffers(1,      &ObjectGPU.vboID);
	ObjectGPU.vboID = 0;
	glDeleteBuffers(1,      &ObjectGPU.iboID);
	ObjectGPU.iboID = 0;
	glDeleteVertexArrays(1, &ObjectGPU.vaoID);
	ObjectGPU.vaoID = 0;
}