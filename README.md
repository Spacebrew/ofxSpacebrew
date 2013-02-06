spacebrew-openframeworks-examples
=================================

Spacebrew openFrameworks examples (including ofxSpacebrew addon)

Setup
=================================
* download openframeworks: http://openframeworks.cc/download
* clone ofxLibwebsockets (https://github.com/labatrockwell/ofxLibwebsockets) in openframeworks/addons
* clone this repo into openframeworks/addons
* follow the steps below to setup ofxSpacebrew + ofxLibwebsockets 

ADDING TO PROJECTS
------------
* OS X
	* Via OF Project Generator:
		1. Delete ofxLibwebsockets/libs/libwebsockets/include/win32port
		2. Add ofxLibwebsockets to your addons via the projectGenerator
		3. Add ofxSpacebrew to your addons via the projectGenerator
		4. That's it!

	* Adding to a new/existing project:
		1. Delete ofxLibwebsockets/libs/libwebsockets/include/win32port
  		2. Add ofxSpacebrew and ofxLibwebsockets code to your project
		3. Include ofxLibwebsockets' xcconfig file via your project's Project.xcconfig file:
			1. Define where it lives: 
			```OFX_LWS_PATH = "$(OF_PATH)/addons/ofxLibwebsockets"```
			2. Include ofxLibwebsockets xcconfig
			```#include "../../../addons/ofxLibwebsockets/ofxLibwebsockets.xcconfig"```
			2. Add to existing vars in Project.xcconfig:
			
			```
			OTHER_LDFLAGS = $(OF_CORE_LIBS) $(OFX_LWS_LIBS)
			HEADER_SEARCH_PATHS = $(OF_CORE_HEADERS) $(OFX_LWS_INCLUDES)
			LIBRARY_SEARCH_PATHS = $(inherited) $(OFX_LWS_SEARCH)
			```

* Windows
	* Adding to new/existing project: 
		1. Add the ofxLibwebsockets source files to the C++ Linker
			* right click on project in the solution explorer, click "Properties", Go down to C++ > General
			* click the arrow at the right of "Additional include directories" and select "edit"
			* add the ofxLibwebsockets source files:
			
			```
			..\..\..\addons\ofxLibwebsockets\libs\jsonpp
			..\..\..\addons\ofxLibwebsockets\libs\jsonpp\json
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include
			..\..\..\addons\ofxLibwebsockets\libs\libwebsockets\include\win32port
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\include
			..\..\..\addons\ofxLibwebsockets\libs\ofxLibwebsockets\src
			..\..\..\addons\ofxLibwebsockets\libs\openssl\openssl
			..\..\..\addons\ofxLibwebsockets\src
```
    2. Add ofxSpacebrew source files to the C++ Linker
      * right click on project in the solution explorer, click "Properties", Go down to C++ > General
			* click the arrow at the right of "Additional include directories" and select "edit"
			* add the ofxSpacebrew source files:
			
			```
			..\..\..\addons\ofxSpacebrew\src
```
