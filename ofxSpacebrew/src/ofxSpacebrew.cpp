//
//  ofxSpacebrew.cpp
//  ofxSpacebrew
//
//  Created by Brett Renfer on 10/30/12.
//
//

#include "ofxSpacebrew.h"

namespace Spacebrew {

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
    
    //--------------------------------------------------------------
    Connection::Connection(){
        bConnected = false;
        _client.addListener(this);
		ofAddListener( ofEvents().update, this, &Connection::update );
		reconnectInterval = 2000;
		bAutoReconnect	  = false;
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

        #ifdef OFX_LWS
            _client.connect( host, SPACEBREW_PORT );
        #else
            stringstream ss;
            ss << "ws://" << host << ":" << SPACEBREW_PORT << "/";
            _client.connect( ss.str() );
        #endif
    }
    
    //--------------------------------------------------------------
    void Connection::connect( string host, Config _config ){
        config = _config;
        
        #ifdef OFX_LWS
            _client.connect( host, SPACEBREW_PORT );
        #else
            stringstream ss;
            ss << "ws://" << host << ":" << SPACEBREW_PORT << "/";
            _client.connect( ss.str() );
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
    void Connection::send( Message m ){
		if ( bConnected ){
            #ifdef OFX_LWS
                _client.send( m.getJSON( config.name ) );
            #else
                if( _client.getClientState() == client::CONNECTED ) {
                    sendString( _client.getConnection(), m.getJSON( config.name ) );
                }
            #endif
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
	}

    //--------------------------------------------------------------
    void Connection::send( Message * m ){
		if ( bConnected ){
            #ifdef OFX_LWS
                _client.send( m->getJSON( config.name ) );
            #else
                if( _client.getClientState() == client::CONNECTED ) {
                    send( m->getJSON( config.name ) );
                }
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
    
#ifdef OFX_LWS
    
    //--------------------------------------------------------------
    void Connection::updatePubSub(){
        _client.send(config.getJSON());
    }
    
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
    void Connection::onIdle( ofxLibwebsockets::Event& args ){
    }



#else
        
    //--------------------------------------------------------------
    void Connection::updatePubSub(){
        if( _client.getClientState() == client::CONNECTED ) {
            sendString( _client.getConnection(), config.getJSON() );
        }
    }
        
    void Connection::onClientSocketMessage( websocketMessageEvent &event ) {
        
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        Json::Value json;
        
        if( _jsonParser.parse( event.message->get_payload(), json ) ) {
            
            if ( !json.isNull() ){
                Message m;
                m.name = json["message"]["name"].asString();
                m.type = json["message"]["type"].asString();
                
                string type = json["message"]["type"].asString();
                if ( type == "string" && json["message"]["value"].isString()){
                    m.value = json["message"]["value"].asString();
                } else if ( type == "boolean" ){
                    if ( json["message"]["value"].isInt() ){
                        m.value = ofToString( json["message"]["value"].asInt());
                    } else if ( json["message"]["value"].isString() ){
                        m.value = json["message"]["value"].asString();
                    }
                } else if ( type == "number" ){
                    if ( json["message"]["value"].isInt() ){
                        m.value = ofToString( json["message"]["value"].asInt());
                    } else if ( json["message"]["value"].isString() ){
                        m.value = json["message"]["value"].asString();
                    }
                } else {
                    stringstream s; s << json["message"]["value"];
                    m.value = s.str();
                }
                
                ofNotifyEvent(onMessageEvent, m, this);
            }
            
        }
    }
    
    void Connection::onClientSocketHandshake(websocketConnectionEvent &event)
    {
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        cout << "handshake!" << endl;
    }
    
    void Connection::onClientSocketOpen(websocketConnectionEvent &event)
    {
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        bConnected = true;
        updatePubSub();
    }
    
    void Connection::onClientSocketClose(websocketConnectionEvent &event)
    {
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        bConnected = false;
		lastTimeTriedConnect = ofGetElapsedTimeMillis();
    }
    
    void Connection::onClientSocketFail(websocketConnectionEvent &event)
    {
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        bConnected = false;
    }
    
    void Connection::onClientSocketPing(websocketPingEvent &event)
    {
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        cout << "ping!" << endl;
    }
    
    void Connection::onClientSocketPong(websocketPingEvent &event)
    {
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        cout << "pong!" << endl;
    }
    
    void Connection::onClientSocketPongFail(websocketPingEvent &event)
    {
        // temporary hack due to global events
        if(event.connection != _client.getConnection()) {
            return;
        }
        
        cout << "pong fail!" << endl;
    }
    
    void Connection::sendString(client::connection_ptr con, const string& str)
    {
        con->send(str);
    }
    
    void Connection::sendBinary(client::connection_ptr con, const string& binary)
    {
        // unfortunately there's no good way to expose the BINARY enum
        // through the wrapper class as it's an Enum
        con->send(binary, websocketpp::frame::opcode::BINARY);
    }
    
#endif
    
    
}