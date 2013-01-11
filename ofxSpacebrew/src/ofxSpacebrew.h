//
//  SpaceBrew.h
//  ofxSpacebrew
//
//  Created by Brett Renfer on 8/17/12.
//
//

#pragma once

#define SPACEBREW_USE_OFX_LWS // prepping to use with a different ws:// library

#ifdef SPACEBREW_USE_OFX_LWS
    #include "ofxLibwebsockets.h"
#else
#endif

#include "ofUtils.h"
#ifndef SPACEBREW_USE_OFX_LWS
    #include "json.h"
#endif

namespace Spacebrew {

    static const int SPACEBREW_PORT = 9000;
    
    class Message {
        public:
            
            Message( string _name="", string _type="", string _val=""){
                name = _name;
                type = _type;
                _default = value = _val;
            }
            
            virtual string getJSON( string configName ){
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
            ~Connection();
        
            void update( ofEventArgs & e );
        
            void connect( string host, string name, string description);
            void connect( string host, Config _config );
        
            void send( string name, string type, string value );
            void send( Message m );
            void send( Message * m );
        
            // edit config
            void addSubscribe( string name, string type );
            void addSubscribe( Message m );
            
            void addPublish( string name, string type, string def="");
            void addPublish( Message m );
        
        #ifdef SPACEBREW_USE_OFX_LWS
            // websocket methods
            void onConnect( ofxLibwebsockets::Event& args );
            void onOpen( ofxLibwebsockets::Event& args );
            void onClose( ofxLibwebsockets::Event& args );
            void onIdle( ofxLibwebsockets::Event& args );
            void onMessage( ofxLibwebsockets::Event& args );
            void onBroadcast( ofxLibwebsockets::Event& args );
        #else
        #endif
            
            Config * getConfig();
        
            bool isConnected();
            void setAutoReconnect( bool bAutoReconnect=true );
            void setReconnectRate( int reconnectMillis );
            bool doesAutoReconnect();
        
            ofEvent<Message> onMessageEvent;
        
        protected:
        
            string host;
            bool bConnected;
            void updatePubSub();
        
            Config config;
            
            // reconnect
            bool bAutoReconnect;
            int  lastTimeTriedConnect;
            int  reconnectInterval;
        
    #ifdef SPACEBREW_USE_OFX_LWS
            ofxLibwebsockets::Client client;
    #else
    #endif
    };
}