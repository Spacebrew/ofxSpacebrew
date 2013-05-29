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
    
    //--------------------------------------------------------------
    bool Message::valueBoolean(){
        if ( type != "boolean" ) ofLogWarning("This Message is not a boolean type! You'll most likely get 'false'");
        return value == "true";
    }
    
    //--------------------------------------------------------------
    int Message::valueRange(){
        if ( type != "range" ) ofLogWarning("This Message is not a range type! Results may be unpredictable");
        return ofClamp(ofToInt(value), 0, 1023);
    }
    
    //--------------------------------------------------------------
    string Message::valueString(){
        if ( type != "string" ) ofLogWarning("This Message is not a string type! Returning raw value as string.");
        return value;
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
    vector<Message> & Config::getPublish(){
        return publish;
    }
    
    //--------------------------------------------------------------
    vector<Message> & Config::getSubscribe(){
        return subscribe;
    }
    
    //--------------------------------------------------------------
    void Config::resetPubSub(){
        publish.clear();
        subscribe.clear();
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
    bool Config::operator == ( Config & comp ){
        if ( clientName == comp.clientName && remoteAddress == comp.remoteAddress ){
            return true;
        }
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
        bConnected = false;
        bAutoReconnect = false;
        ofRemoveListener( ofEvents().update, this, &Connection::update );
        
#ifdef SPACEBREW_USE_OFX_LWS
        client.close();
#endif
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
            
            if ( bConnected ) ofNotifyEvent(onMessageEvent, m, this);
        }
    }
    
    //--------------------------------------------------------------
    void Connection::onBroadcast( ofxLibwebsockets::Event& args ){
        
    }

#endif

#pragma mark Route
    //--------------------------------------------------------------
    Route::Route( RouteEndpoint pub, RouteEndpoint sub ){
        updatePublisher( pub );
        updateSubscriber( sub );
    }
    
    //--------------------------------------------------------------
    void Route::updatePublisher( RouteEndpoint pub ){
        publisher = pub;
    }
    
    //--------------------------------------------------------------
    void Route::updateSubscriber( RouteEndpoint sub ){
        subscriber = sub;
    }
    
    //--------------------------------------------------------------
    RouteEndpoint Route::getPublisher(){
        return publisher;
    }
    
    //--------------------------------------------------------------
    RouteEndpoint Route::getSubscriber(){
        return  subscriber;
    }
    
    //--------------------------------------------------------------
    bool Route::operator == ( Route & r){
        RouteEndpoint pub = r.getPublisher();
        RouteEndpoint sub = r.getSubscriber();
        
        bool bSamePub = pub.clientName == publisher.clientName &&
                        pub.name == publisher.name &&
                        pub.type == publisher.type &&
                        pub.remoteAddress == publisher.remoteAddress;
    
        bool bSameSub = sub.clientName == subscriber.clientName &&
                        sub.name == subscriber.name &&
                        sub.type == subscriber.type &&
                        sub.remoteAddress == subscriber.remoteAddress;
        
        return bSamePub && bSameSub;
    }
    
#pragma mark AdminConnection
    
#ifdef SPACEBREW_USE_OFX_LWS

    //--------------------------------------------------------------
    void AdminConnection::onOpen( ofxLibwebsockets::Event& args ){
        Connection::onOpen(args);
        
        // send admin "register" message
        client.send("{\"admin\":[{\"admin\": true,\"no_msgs\": true}]}");
    }

    //--------------------------------------------------------------
    void AdminConnection::onMessage( ofxLibwebsockets::Event& args ){
        if ( !args.json.isNull() ){
            
            // start configs come in as array
            if ( args.json.isArray() ){
                for (int k=0; k<args.json.size(); k++){
                    
                    Json::Value config = args.json[k];
                    
                    processIncomingJson( config );
                }
            // normal ws event
            } else if ( !args.json["message"].isNull() && args.json["message"]["clientName"].isNull()){
                Connection::onMessage(args);
            
            // 
            } else {
                processIncomingJson( args.json );
            }
        }
    }
    
    //--------------------------------------------------------------
    void AdminConnection::processIncomingJson( Json::Value & config ){
        // new connection
        if ( !config["config"].isNull() ){
            Config c;
            c.name          = config["config"]["name"].asString();
            c.description   = config["config"]["description"].asString();
            c.remoteAddress = config["config"]["remoteAddress"].asString();
            
            Json::Value publishes = config["config"]["publish"]["messages"];
            for ( int i=0; i<publishes.size(); i++){
                c.addPublish(publishes[i]["name"].asString(), publishes[i]["type"].asString(), publishes[i]["default"].asString());
            }
            
            Json::Value subscribes = config["config"]["subscribe"]["messages"];
            for ( int i=0; i<subscribes.size(); i++){
                c.addSubscribe(subscribes[i]["name"].asString(), subscribes[i]["type"].asString());
            }
            
            bool bNew = true;
            
            // does this client exist yet?
            for (int j=0; j<connectedClients.size(); j++){
                if ( connectedClients[j] == c)
                {
                    connectedClients[j] = c;
                    ofNotifyEvent(onClientUpdatedEvent, connectedClients[j], this);
                    bNew = false;
                    break;
                }
            }
            
            if ( bNew ){
                // doesn't exist yet, add as new
                connectedClients.push_back( c );
                ofNotifyEvent(onClientConnectEvent, c, this);
            }
            
            // connection removed
        } else if ( !config["remove"].isNull()){
            
            for (int i=0; i < config["remove"].size(); i++){
                
                Json::Value toRemove = config["remove"][i];
                string name          = toRemove["name"].asString();
                string remoteAddress = toRemove["remoteAddress"].asString();
                
                for (int j=0; j<connectedClients.size(); j++){
                    if ( connectedClients[j].name == name &&
                        connectedClients[j].remoteAddress == remoteAddress)
                    {
                        ofNotifyEvent(onClientDisconnectEvent, connectedClients[j], this);
                        connectedClients.erase(connectedClients.begin() + j );
                        break;
                    }
                }
            }
            // route
        } else if ( !config["route"].isNull()){
            
            RouteEndpoint pub;
            pub.name            = config["route"]["publisher"]["name"].asString();
            pub.type            = config["route"]["publisher"]["type"].asString();
            pub.clientName      = config["route"]["publisher"]["clientName"].asString();
            pub.remoteAddress   = config["route"]["publisher"]["remoteAddress"].asString();
            
            RouteEndpoint sub;
            sub.name            = config["route"]["subscriber"]["name"].asString();
            sub.type            = config["route"]["subscriber"]["type"].asString();
            pub.clientName      = config["route"]["subscriber"]["clientName"].asString();
            sub.remoteAddress   = config["route"]["subscriber"]["remoteAddress"].asString();
            
            Route r( pub, sub );
            
            if ( config["route"]["type"].asString() == "add" ){
                currentRoutes.push_back(r);
                ofNotifyEvent(onRouteAdded, r, this);
            } else if ( config["route"]["type"].asString() == "remove"){
                for (int i=0; i<currentRoutes.size(); i++){
                    if ( currentRoutes[i] == r ){
                        ofNotifyEvent(onRouteRemoved, r, this);
                        break;
                    }
                }
            }
            
            // data
        } else if ( !config["message"].isNull()){
            
            // message from admin
            if ( !config["message"]["clientName"].isNull()){
                DataMessage m;
                m.clientName    = config["message"]["clientName"].asString();
                m.remoteAddress = config["message"]["remoteAddress"].asString();
                m.name          = config["message"]["name"].asString();
                m.type          = config["message"]["type"].asString();
                m.value         = config["message"]["value"].asString();
            }
        }
    }
#endif
    
}