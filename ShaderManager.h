//====================================================
// @Yanshi
// Modified from 
// https://github.com/OrionQuest/Nova_Examples/blob/master/opengl/ShaderManager.h
//====================================================

#ifndef OGL_SHADER_MANAGER_H__
#define OGL_SHADER_MANAGER_H__

#include "Shader.h"
#include "ResourceManager.h"
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace opengl_gui_viewer {

    class ShaderManager : public AbstractResourceManager {
    public:
        ShaderManager();
        virtual ~ShaderManager();
        
        std::unique_ptr<Shader> GetShader(const std::string& name);

    // private:
        
        std::string ReadShader(std::string type, std::string path);
        unsigned int LoadFromFiles( std::string name );
        unsigned int LoadFromString( const char* vertex_shader,
                                     const char* fragment_shader,
                                     const char* geometry_shader );

        std::map<std::string, unsigned int> _shaderRepo;
    };
}

#endif
