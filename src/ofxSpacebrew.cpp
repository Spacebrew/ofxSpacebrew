//
//  ofxSpacebrew.cpp
//  ofxSpacebrew
//
//  Created by Brett Renfer on 10/30/12.
//
//

#include "ofxSpacebrew.h"

namespace Spacebrew {
    
#pragma mark Message
    
    //--------------------------------------------------------------
    Message::Message( string _name, string _type, string _val){
        name = _name;
        type = _type;
        _default = value = _val;
    }
    
    //--------------------------------------------------------------
    string Message::getJSON( string configName ){
        if ( type == "string" || type == "boolean" ){
            return "{\"message\":{\"clientName\":\"" + configName +"\",\"name\":\"" + name + "\",\"type\":\"" + type + "\",\"value\":\"" + value +"\"}}";
        } else {
            return "{\"message\":{\"clientName\":\"" + configName +"\",\"name\":\"" + name + "\",\"type\":\"" + type + "\",\"value\":" + value +"}}";
        }
    }
    
#pragma mark Config
    
    //--------------------------------------------------------------
    void Config::addSubscribe( string name, string type ){
        subscribe.push_back( Message(name, type) );
    }
    
    //--------------------------------------------------------------
    void Config::addSubscribe( Message m ){
        subscribe.push_back(m);
    }

    //--------------------------------------------------------------
    void Config::addPublish( string name, string type, string def){
        publish.push_back( Message(name, type, def) );
    }
    
    //--------------------------------------------------------------
    void Config::addPublish( Message m ){
        publish.push_back(m);
    }
    
    //--------------------------------------------------------------
    string Config::getJSON(){
        string message = "{\"config\": {\"name\": \"" + name +"\",\"description\":\"" + description +"\",\"publish\": {\"messages\": [";
        
        for (int i=0, len=publish.size(); i<len; i++){
            message += "{\"name\":\"" + publish[i].name + "\",";
            message += "\"type\":\"" + publish[i].type + "\",";
            message += "\"default\":\"" + publish[i].value + "\"";
            message += "}";
            if ( i+1 < len ){
                message += ",";
            }
        }
        
        message += "]},\"subscribe\": {\"messages\": [";
        
        for (int i=0, len=subscribe.size(); i<len; i++){
            message += "{\"name\":\"" + subscribe[i].name + "\",";
            message += "\"type\":\"" + subscribe[i].type + "\"";
            message += "}";
            if ( i+1 < len ){
                message += ",";
            }
        }
        
        message += "]}}}";
        
        return message;
    }
    
#pragma mark Connection
    
    //--------------------------------------------------------------
    Connection::Connection(){
        bConnected = false;
    #ifdef SPACEBREW_USE_OFX_LWS
        client.addListener(this);
    #endif

        ofAddListener( ofEvents().update, this, &Connection::update );
        reconnectInterval = 2000;
        bAutoReconnect    = false;
    }

    //--------------------------------------------------------------
    Connection::~Connection(){
        ofRemoveListener( ofEvents().update, this, &Connection::update );
    }
        
    //--------------------------------------------------------------
    void Connection::update( ofEventArgs & e ){
        if ( bAutoReconnect ){
            if ( !bConnected && ofGetElapsedTimeMillis() - lastTimeTriedConnect > reconnectInterval ){
                lastTimeTriedConnect = ofGetElapsedTimeMillis();
                connect( host, config );
            }
        }
    }

    //--------------------------------------------------------------
    void Connection::connect( string _host, string name, string description){
        host = _host;
        config.name = name;
        config.description = description;

    #ifdef SPACEBREW_USE_OFX_LWS
        client.connect( host, SPACEBREW_PORT );
    #endif
    }
    
    //--------------------------------------------------------------
    void Connection::connect( string host, Config _config ){
        config = _config;
    #ifdef SPACEBREW_USE_OFX_LWS
        client.connect( host, SPACEBREW_PORT );
    #endif
    }
    
    //--------------------------------------------------------------
    void Connection::send( string name, string type, string value ){
        if ( bConnected ){
            Message m( name, type, value);
			send(m);
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
    }

    //--------------------------------------------------------------
    void Connection::sendString( string name, string value ){
        if ( bConnected ){
            Message m( name, "string", value);
            send(m);
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
    }

    //--------------------------------------------------------------
    void Connection::sendRange( string name, int value ){
        if ( bConnected ){
            Message m( name, "range", ofToString(value));
            send(m);
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
    }

    //--------------------------------------------------------------
    void Connection::sendBoolean( string name, bool value ){
        if ( bConnected ){
            string out = value ? "true" : "false";
            Message m( name, "boolean", out);
            send(m);
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
    }

    //--------------------------------------------------------------
    void Connection::send( Message m ){
		if ( bConnected ){
        #ifdef SPACEBREW_USE_OFX_LWS
            client.send( m.getJSON( config.name ) );
        #endif
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
	}

    //--------------------------------------------------------------
    void Connection::send( Message * m ){
		if ( bConnected ){
        #ifdef SPACEBREW_USE_OFX_LWS
            client.send( m->getJSON( config.name ) );
        #endif
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
	}
    
    //--------------------------------------------------------------
    void Connection::addSubscribe( string name, string type ){
        config.addSubscribe(name, type);
        if ( bConnected ){
            updatePubSub();
        }
    }
    
    //--------------------------------------------------------------
    void Connection::addSubscribe( Message m ){
        config.addSubscribe(m);
        if ( bConnected ){
            updatePubSub();
        }
    }
    
    //--------------------------------------------------------------
    void Connection::addPublish( string name, string type, string def){
        config.addPublish(name, type, def);
        if ( bConnected ){
            updatePubSub();
        }
    }
    
    //--------------------------------------------------------------
    void Connection::addPublish( Message m ){
        config.addPublish(m);
        if ( bConnected ){
            updatePubSub();
        }
    }

    //--------------------------------------------------------------
    Config * Connection::getConfig(){
        return &config;
    }
    
    //--------------------------------------------------------------
    bool Connection::isConnected(){
        return bConnected;
    }

    //--------------------------------------------------------------
    void Connection::setAutoReconnect( bool _bAutoReconnect ){
        bAutoReconnect = _bAutoReconnect;
    }

    //--------------------------------------------------------------
    void Connection::setReconnectRate( int reconnectMillis ){
        reconnectInterval = reconnectMillis;
    }

    //--------------------------------------------------------------
    bool Connection::doesAutoReconnect(){
        return bAutoReconnect;
    }

    //--------------------------------------------------------------
    void Connection::updatePubSub(){
#ifdef SPACEBREW_USE_OFX_LWS
        client.send(config.getJSON());
#endif
    }
    
    //--------------------------------------------------------------
    string Connection::getHost(){
        return host;
    }

#ifdef SPACEBREW_USE_OFX_LWS
    
    //--------------------------------------------------------------
    void Connection::onConnect( ofxLibwebsockets::Event& args ){
    }
    
    //--------------------------------------------------------------
    void Connection::onOpen( ofxLibwebsockets::Event& args ){
        bConnected = true;
        updatePubSub();
    }
    
    //--------------------------------------------------------------
    void Connection::onClose( ofxLibwebsockets::Event& args ){
        bConnected = false;
        lastTimeTriedConnect = ofGetElapsedTimeMillis();
    }
    
    //--------------------------------------------------------------
    void Connection::onIdle( ofxLibwebsockets::Event& args ){}
    
    //--------------------------------------------------------------
    void Connection::onMessage( ofxLibwebsockets::Event& args ){
        if ( !args.json.isNull() ){
            Message m;
            m.name = args.json["message"]["name"].asString();
            m.type = args.json["message"]["type"].asString();
            
            string type = args.json["message"]["type"].asString();
            if ( type == "string" && args.json["message"]["value"].isString()){
                m.value = args.json["message"]["value"].asString();
            } else if ( type == "boolean" ){
                if ( args.json["message"]["value"].isInt() ){
                    m.value = ofToString( args.json["message"]["value"].asInt());
                } else if ( args.json["message"]["value"].isString() ){
                    m.value = args.json["message"]["value"].asString();
                }
            } else if ( type == "number" ){
                if ( args.json["message"]["value"].isInt() ){
                    m.value = ofToString( args.json["message"]["value"].asInt());
                } else if ( args.json["message"]["value"].isString() ){
                    m.value = args.json["message"]["value"].asString();
                }
            } else {
				stringstream s; s<<args.json["message"]["value"];
				m.value = s.str();
            }
            
            ofNotifyEvent(onMessageEvent, m, this);
        }
    }
    
    //--------------------------------------------------------------
    void Connection::onBroadcast( ofxLibwebsockets::Event& args ){
        
    }

#endif
}