#include <gazebo/gazebo.hh>
#include <stdlib.h>

namespace gazebo
{
	class Project1Plugin : public WorldPlugin
	{
		public: Project1Plugin() : WorldPlugin()
		{
			char* username;
			username = getenv("LOGNAME");		
			
			if(username)
			{
				printf("Hello, %s, welcome to Nipuna's building.\n",username);
			}
			else
			{
				printf("Hello, welcome to Nipuna's building.\n");
			}
		}
		public: void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
        {
   	    }
       };
       GZ_REGISTER_WORLD_PLUGIN(Project1Plugin)
}	  
