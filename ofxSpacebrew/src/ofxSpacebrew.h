//
//  SpaceBrew.h
//  ofxSpacebrew
//
//  Created by Brett Renfer on 8/17/12.
//
//

#pragma once

#define OFX_LWS // prepping to use with a different ws:// library

#ifdef OFX_LWS
#include "ofxLibwebsockets.h"
#else
#endif

#include "ofUtils.h"

namespace Spacebrew {
    
    class Message {
        public:
            
            Message( string _name="", string _type="", string _val=""){
                name = _name;
                type = _type;
                _default = value = _val;
            }
            
            string getJSON( string configName ){
                return "{\"message\":{\"clientName\":\"" + configName +"\",\"name\":\"" + name + "\",\"type\":\"" + type + "\",\"value\":\"" + value +"\"}}";
            }
            
            string name, type, _default, value;
    };
    
    class Config {
        public:
            
            void addSubscribe( string name, string type );
            void addSubscribe( Message m );
        
            void addPublish( string name, string type, string def);
            void addPublish( Message m );
        
            string getJSON();
            string name, description;
            
        private:
            
            vector<Message> publish;
            vector<Message> subscribe;
    };
    
    class Connection {
        public:
            Connection();
        
            void connect( string host, string name, string description);
            void connect( string host, Config _config );
        
            void send( string name, string type, string value );
        
            // edit config
            void addSubscribe( string name, string type );
            void addSubscribe( Message m );
            
            void addPublish( string name, string type, string def);
            void addPublish( Message m );
        
            // websocket methods
            void onConnect( ofxLibwebsockets::Event& args );
            void onOpen( ofxLibwebsockets::Event& args );
            void onClose( ofxLibwebsockets::Event& args );
            void onIdle( ofxLibwebsockets::Event& args );
            void onMessage( ofxLibwebsockets::Event& args );
            void onBroadcast( ofxLibwebsockets::Event& args );
            
            Config * getConfig();
        
            bool isConnected();
        
            ofEvent<Message> onMessageEvent;
        
        protected:
        
            bool bConnected;
            void updatePubSub();
        
            Config config;
        
    #ifdef OFX_LWS
            ofxLibwebsockets::Client client;
    #else
    #endif
    };
}