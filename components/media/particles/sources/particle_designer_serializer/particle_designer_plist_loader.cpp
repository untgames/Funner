#include "shared.h"

using namespace media::particles;
using namespace common;

namespace components
{
    
    namespace particle_designer_plist_loader
    {
        
        /*
         Constants
         */
        
        const char* COMPONENT_NAME = "media.particles.loaders.particle_designer_plist"; //component name
        
        /*
         Particles library loader for particle designer plist format
         */
        
        class PlistLibraryLoader
        {
        public:
            PlistLibraryLoader (const char* file_name, ParticleSystemLibrary& out_library)
            : parser (file_name, "xml")
            {
                //logging
                Log log (LOG_NAME);
                
                parser.Log ().Print (xtl::bind (&Log::Print, xtl::ref (log), _1));
                
                if (parser.Root ().First ("array"))
                    ParseSceneArray (parser.Root ().First ("array"));
                else
                    ParseEmitter (parser.Root ().First ("dict"));
            }
            
        private:
            void ParseSceneArray (Parser::Iterator scene_array_iter)
            {
                
            }
            
            void ParseEmitter (Parser::Iterator emitter_iter)
            {
                
            }
            
        private:
            Parser parser;   //parser
        };
        
        /*
         Component
         */
        
        class Component
        {
        public:
            Component ()
            {
                ParticleSystemLibraryManager::RegisterLoader ("plist", &LoadLibrary);
            }
            
        private:
            static void LoadLibrary (const char* name, ParticleSystemLibrary& library)
            {
                PlistLibraryLoader (file_name, library);
            }
        };
        
        extern "C"
        {
            
            ComponentRegistrator<Component> ParticleDesignerPlistLoader (COMPONENT_NAME);
            
        }
        
    }
    
}
