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
    Message::Message( string _name, string _type, string _val) :
    name(_name),
    type(_type),
    _default(_val),
    value(_val)
    {
        bUseReference = false;
    }
    
    //--------------------------------------------------------------
    Message::Message( string _name, string _type, string * _val ) :
    name(_name),
    type(_type),
    _default(*_val),
    valuePtr(_val)
    {
        bUseReference = true;
    }
    
    //--------------------------------------------------------------
    Message::Message( string _name, string _type, int * _val ) :
    name(_name),
    type(_type),
    _default(ofToString(*_val)),
    valuePtr(_val)
    {
        bUseReference = true;
    }
    
    //--------------------------------------------------------------
    Message::Message( string _name, string _type, bool * _val ) :
    name(_name),
    type(_type),
    _default(ofToString(*_val)),
    valuePtr(_val)
    {
        bUseReference = true;
    }
    
    //--------------------------------------------------------------
    Message::Message( string _name, string _type, void * _val ) :
    name(_name),
    type(_type),
    valuePtr(_val)
    {
        bUseReference = true;
        _default = "";
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
        if ( bUseReference ){
            return  *((bool*)valuePtr);
        } else {
            return value == "true";
        }
    }
    
    //--------------------------------------------------------------
    int Message::valueRange(){
        if ( type != "range" ) ofLogWarning("This Message is not a range type! Results may be unpredictable");
        if ( bUseReference ){
            return  *((int*)valuePtr);
        } else {
            if ( ofToInt(value) > 1023 || ofToInt(value) < 0 ) ofLogWarning()<<"Value is outside of Range: 0-1023";
            return ofClamp(ofToInt(value), 0, 1023);
        }
    }
    
    //--------------------------------------------------------------
    string Message::valueString(){
        if ( type != "string" ) ofLogWarning("This Message is not a string type! Returning raw value as string.");
        if ( bUseReference ){
            return  *((string*)valuePtr);
        } else {
            return value;
        }
    }
    
    //--------------------------------------------------------------
    void Message::update( string val ){
        // update pointer's value if necessary
        if (bUseReference){
            if ( type == TYPE_STRING ){
                *(string*)valuePtr = val;
            } else if ( type == TYPE_RANGE ){
                *(int*)valuePtr = ofToInt(val);
            } else if ( type == TYPE_BOOLEAN ){
                *(bool*)valuePtr = ofToBool(val);
            } else {
                // arg... this should be a template
                //*valuePtr = (void) val;
            }
        }
        // update basic value
        value = val;
    }
    
    //--------------------------------------------------------------
    bool Message::operator == ( Message & comp ){
        return (name == comp.name && type == comp.type);
    }
    
#pragma mark BinaryMessage
    
    //--------------------------------------------------------------
    BinaryMessage::BinaryMessage( string _name, string _type, const ofBuffer buff )
    : Message(_name, _type){
        buffer.clear();
        buffer.append(buff.getBinaryBuffer(), buff.size());
    }
    
    //--------------------------------------------------------------
    BinaryMessage::BinaryMessage( string _name, string _type, const char * data, const long size )
    : Message(_name, _type){
        buffer.clear();
        buffer.append(data, size);
    }
    
    //--------------------------------------------------------------
    string BinaryMessage::getJSON( string configName ){
        stringstream ss;
        ss<<"{\"message\":{\"clientName\":\"";
        ss<<configName<<"\",\"name\":\""<<name + "\",";
        ss<<"\"type\":\"" + type + "\",";
        ss<<"\"value\":"<<buffer.size()<<"}}";
        return ss.str();
    }
    
    //--------------------------------------------------------------
    ofBuffer BinaryMessage::getSendBuffer( string configName ){
        outputBuffer.clear();
        string json = getJSON(configName);
        unsigned int jsonByteLength = json.length();
        int numBytesForJsonLength = (jsonByteLength > 0xFFFF ? 5 : (jsonByteLength >= 254 ? 3 : 1));
        if (numBytesForJsonLength == 5){
            char c[] = {255};
            outputBuffer.append(c, 1);
            char len[] = {jsonByteLength >> 24, jsonByteLength >> 16, jsonByteLength >> 8, jsonByteLength};
            outputBuffer.append(len, 4);
        } else if (numBytesForJsonLength == 3){
            char c[] = {254};
            outputBuffer.append(c, 1);
            char len[] = {jsonByteLength >> 8, jsonByteLength};
            outputBuffer.append(len, 2);
        } else {
            char len[] = {jsonByteLength};
            outputBuffer.append(len, 1);
        }
        outputBuffer.append( json );
        outputBuffer.append( buffer.getBinaryBuffer(), buffer.size() );
        return outputBuffer;
    }
    
    //--------------------------------------------------------------
    ofBuffer & BinaryMessage::data(){
        return buffer;
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
    void Config::addSubscribe( string name, string type, int * value ){
        subscribe.push_back( Message(name, type, value) );
    }
    
    //--------------------------------------------------------------
    void Config::addSubscribe( string name, string type, bool * value ){
        subscribe.push_back( Message(name, type, value) );
    }
    
    //--------------------------------------------------------------
    void Config::addSubscribe( string name, string type, void * value ){
        subscribe.push_back( Message(name, type, value) );
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
        string message = "{\"config\": {\"name\": \"" + clientName +"\",\"description\":\"" + description +"\",\"publish\": {\"messages\": [";
        
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
        return false;
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
        config.clientName = name;
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
            client.send( m.getJSON( config.clientName ) );
        #endif
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
	}

    //--------------------------------------------------------------
    void Connection::send( Message * m ){
		if ( bConnected ){
        #ifdef SPACEBREW_USE_OFX_LWS
            client.send( m->getJSON( config.clientName ) );
        #endif
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
	}
    
    //--------------------------------------------------------------
    void Connection::sendBinary( string name, string type, ofBuffer data ){
        if ( bConnected ){
            BinaryMessage m( name, type, data);
            sendBinary(m);
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
    }
    
    //--------------------------------------------------------------
    void Connection::sendBinary( BinaryMessage m ){
        if ( bConnected ){
        #ifdef SPACEBREW_USE_OFX_LWS
            client.sendBinary( m.getSendBuffer( config.clientName ) );
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
    void Connection::addSubscribe( string name, string * value ){
        config.addSubscribe(name, TYPE_STRING, value);
        if ( bConnected ){
            updatePubSub();
        }
    }
    
    //--------------------------------------------------------------
    void Connection::addSubscribe( string name, bool * value ){
        config.addSubscribe(name, TYPE_BOOLEAN, value);
        if ( bConnected ){
            updatePubSub();
        }
    }
    
    //--------------------------------------------------------------
    void Connection::addSubscribe( string name, int * value ){
        config.addSubscribe(name, TYPE_RANGE, value);
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
    ofxLibwebsockets::Client & Connection::getWebsocket(){
        return client;
    }
    
    //--------------------------------------------------------------
    void Connection::onConnect( ofxLibwebsockets::Event& args ){
    }
    
    //--------------------------------------------------------------
    void Connection::onOpen( ofxLibwebsockets::Event& args ){
        bConnected = true;
        updatePubSub();
        ofNotifyEvent( onOpenEvent, args, this);
    }
    
    //--------------------------------------------------------------
    void Connection::onClose( ofxLibwebsockets::Event& args ){
        bConnected = false;
        lastTimeTriedConnect = ofGetElapsedTimeMillis();
        ofNotifyEvent( onCloseEvent, args, this);
    }
    
    //--------------------------------------------------------------
    void Connection::onIdle( ofxLibwebsockets::Event& args ){}
    
    //--------------------------------------------------------------
    void Connection::onMessage( ofxLibwebsockets::Event& args ){
        if ( !args.isBinary ){
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
                } else if ( type == "number" || type == "range" ){
                    if ( args.json["message"]["value"].isInt() ){
                        m.value = ofToString( args.json["message"]["value"].asInt());
                    } else if ( args.json["message"]["value"].isString() ){
                        m.value = args.json["message"]["value"].asString();
                    }
                } else {
                    stringstream s; s<<args.json["message"]["value"];
                    m.value = s.str();
                }
                
                // find message in config and update if necessary
                vector<Message> sub = config.getSubscribe();
                for ( int i=0; i<sub.size(); i++){
                    if ( sub[i] == m ){
                        sub[i].update( m.value );
                    }
                }
                
                if ( bConnected ) ofNotifyEvent(onMessageEvent, m, this);
            }
        } else {
//            string temp(args.data.getBinaryBuffer());
            
            // first, extract the message
            if ( args.data.size() > 0 ){
                unsigned long jsonLength = args.data.getBinaryBuffer()[0];
                int jsonStartIndex = 1;
                if (jsonLength == 254){
                    if (args.data.size() > 3){
                        jsonLength = args.data.getBinaryBuffer()[1];
                        jsonStartIndex = 3;
                    } else {
                        ofLogError()<<"[ofxSpacebrew::Connection] Binary message of incorrect format";
                        return;
                    }
                } else if (jsonLength == 255){
                    if (args.data.size() > 5){
                        jsonLength = args.data.getBinaryBuffer()[1];
                        jsonStartIndex = 5;
                    } else {
                        ofLogError()<<"[ofxSpacebrew::Connection] Binary message of incorrect format";
                        return;
                    }
                }
                
                if (jsonLength > 0 ){
                    string jsonStr = args.data.getText().substr(jsonStartIndex, jsonLength);
                    
                    static Json::Reader jsonReader;
                    Json::Value json;
                    jsonReader.parse(jsonStr, json);
                    
                    string name = json["message"]["name"].asString();
                    string type = json["message"]["type"].asString();
                    
                    // value == size of binary data
                    stringstream s; s<<json["message"]["value"];
                    string value = s.str();
                    
                    // find message in config and update if necessary
                    //                vector<Message> sub = config.getSubscribe();
                    //                for ( int i=0; i<sub.size(); i++){
                    //                    if ( sub[i] == m ){
                    //                        sub[i].update( m.value );
                    //                    }
                    //                }
                    
                    int size = ofToInt(value);
                    char * data = (char*) calloc(size, sizeof(char) );
                    memcpy(data, args.data.getBinaryBuffer() + jsonStartIndex + jsonLength, size);
                    
                    BinaryMessage m(name, type, data, size);
                    
                    ofLogVerbose() << "[ofxSpacebrew::Connection] got binary of size "<<size;;
                    
                    if ( bConnected ) ofNotifyEvent(onBinaryMessage, m, this);
                } else {
                    ofLogError() << "[ofxSpacebrew::Connection] got bad binary frame (no json)";
                }
                    
//                    if (args.data.size() >= jsonStartIndex + jsonLength) {
//                        try {
//                            var json  = JSON.parse( message.slice(jsonStartIndex, jsonStartIndex + jsonLength ).toString());
//                        } catch ( err ){
//                            logger.log("error", "[wss.onmessage] Error parsing JSON from binary packet. Discarding");
//                            return;
//                        }
//                        
//                        bValidMessage = handleBinaryMessage(connection, json, message.slice(jsonStartIndex + jsonLength) );
//                    } else {
//                        logger.log("error", "[wss.onmessage] message of incorrect format");
//                        return;
//                    }
            }
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
            } else if ( !args.json["message"].isNull()
                       && (args.json["message"]["targetType"].isNull() || args.json["message"]["targetType"].asString() == "client")){
                Connection::onMessage(args);
            
            // admin event
            } else {
                processIncomingJson( args.json );
            }
        }
    }
    
    //--------------------------------------------------------------
    bool AdminConnection::addRoute( string pub_client, string pub_address, string pub_name,
                  string sub_client, string sub_address, string sub_name )
    {
        //find in routes list
        RouteEndpoint pub;
        pub.clientName = pub_client;
        pub.remoteAddress = pub_address;
        pub.name = pub_name;
        
        RouteEndpoint sub;
        sub.clientName = sub_client;
        sub.remoteAddress = sub_address;
        sub.name = sub_name;
        
        bool bValidPublisher = false;
        bool bValidSubscriber = false;
        
        for (int i=0; i<connectedClients.size(); i++){
            if ( connectedClients[i].clientName == pub_client &&
                 connectedClients[i].remoteAddress == pub_address ){
                
                // make sure it's a real publisher
                for ( int j=0; j<connectedClients[i].getPublish().size(); j++){
                    if ( connectedClients[i].getPublish()[j].name == pub_name ){
                        pub.type = connectedClients[i].getPublish()[j].type;
                        bValidPublisher = true;
                        break;
                    }
                }
            } else if ( connectedClients[i].clientName == sub_client &&
                       connectedClients[i].remoteAddress == sub_address ){
            
                // make sure it's a real subscriber
                for ( int j=0; j<connectedClients[i].getSubscribe().size(); j++){
                    if ( connectedClients[i].getSubscribe()[j].name == pub_name ){
                        sub.type = connectedClients[i].getSubscribe()[j].type;
                        bValidSubscriber = true;
                        break;
                    }
                }
            }
        }
        if ( !bValidPublisher || !bValidSubscriber ){
            ofLogWarning()<<( !bValidPublisher ? "Invalid publisher!\n" : "")<<(!bValidSubscriber ? "Invalid subscriber!" : "");
            return false;
        } else {
            updateRoute(ADD_ROUTE, Route( pub, sub ));
            return true;
        }
    }
    
    //--------------------------------------------------------------
    bool AdminConnection::addRoute( RouteEndpoint pub_endpoint, RouteEndpoint sub_endpoint )
    {
        bool bValidPublisher = false;
        bool bValidSubscriber = false;
        
        //find in routes list
        for (int i=0; i<connectedClients.size(); i++){
            if ( connectedClients[i].clientName == pub_endpoint.clientName &&
                connectedClients[i].remoteAddress == pub_endpoint.remoteAddress ){
                // make sure it's a real publisher
                for ( int j=0; j<connectedClients[i].getPublish().size(); j++){
                    if ( connectedClients[i].getPublish()[j].name == pub_endpoint.name ){
                        bValidPublisher = true;
                        break;
                    }
                }
            } else if ( connectedClients[i].clientName == sub_endpoint.clientName &&
                       connectedClients[i].remoteAddress == sub_endpoint.remoteAddress ){
            
                // make sure it's a real subscriber
                for ( int j=0; j<connectedClients[i].getSubscribe().size(); j++){
                    if ( connectedClients[i].getSubscribe()[j].name == sub_endpoint.name ){
                        bValidSubscriber = true;
                        break;
                    }
                }
            }
        }
        if ( !bValidPublisher || !bValidSubscriber ){
            ofLogWarning()<<( !bValidPublisher ? "Invalid publisher! " : "")<<(!bValidSubscriber ? "Invalid subscriber!" : "");
            return false;
        } else {
            updateRoute(ADD_ROUTE, Route( pub_endpoint, sub_endpoint ));
            return true;
        }
    }
    
    //--------------------------------------------------------------
    bool AdminConnection::addRoute( Route route ){
        bool bValidRoute = false;
        
        //find in routes list
        for (int i=0; i<currentRoutes.size(); i++){
            if ( currentRoutes[i] == route ){
                bValidRoute = true;
            }
        }
        if ( !bValidRoute ){
            ofLogWarning()<<"Invalid route!";
            return false;
        } else {
            updateRoute(ADD_ROUTE, route );
            return true;
        }
    }
    
    //--------------------------------------------------------------
    bool AdminConnection::removeRoute( string pub_client, string pub_address, string pub_name,
                     string sub_client, string sub_address, string sub_name )
    {
        //find in routes list
        RouteEndpoint pub;
        RouteEndpoint sub;
        
        bool bValidPublisher = false;
        bool bValidSubscriber = false;
        
        for (int i=0; i<currentRoutes.size(); i++){
            if ( currentRoutes[i].getPublisher().clientName == pub_client &&
                currentRoutes[i].getPublisher().name == pub_name &&
                currentRoutes[i].getPublisher().remoteAddress == pub_address ){
                pub = currentRoutes[i].getPublisher();
                bValidPublisher = true;
            }
            if ( currentRoutes[i].getSubscriber().clientName == sub_client &&
                currentRoutes[i].getSubscriber().name == sub_name &&
                currentRoutes[i].getSubscriber().remoteAddress == sub_address ){
                sub = currentRoutes[i].getSubscriber();
                bValidSubscriber = true;
            }
        }
        if ( !bValidPublisher || !bValidSubscriber ){
            ofLogWarning()<<( !bValidPublisher ? "Invalid publisher! " : "")<<(!bValidSubscriber ? "Invalid subscriber!" : "");
            return false;
        } else {
            updateRoute(REMOVE_ROUTE, Route( pub, sub ));
            return true;
        }
    }
    
    //--------------------------------------------------------------
    bool AdminConnection::removeRoute( RouteEndpoint pub_endpoint, RouteEndpoint sub_endpoint ){
        bool bValidPublisher = false;
        bool bValidSubscriber = false;
        
        //find in routes list
        for (int i=0; i<currentRoutes.size(); i++){
            if ( currentRoutes[i].getPublisher() == pub_endpoint ){
                bValidPublisher = true;
            }
            if ( currentRoutes[i].getSubscriber() == sub_endpoint ){
                bValidSubscriber = true;
            }
        }
        if ( !bValidPublisher || !bValidSubscriber ){
            ofLogWarning()<<( !bValidPublisher ? "Invalid publisher!\n" : "")<<(!bValidSubscriber ? "Invalid subscriber!" : "");
            return false;
        } else {
            updateRoute(REMOVE_ROUTE, Route( pub_endpoint, sub_endpoint ));
            return true;
        }
    }
    
    //--------------------------------------------------------------
    bool AdminConnection::removeRoute( Route route ){
        bool bValidRoute = false;
        
        //find in routes list
        for (int i=0; i<currentRoutes.size(); i++){
            if ( currentRoutes[i] == route ){
                bValidRoute = true;
            }
        }
        if ( !bValidRoute ){
            ofLogWarning()<<"Invalid route!";
            return false;
        } else {
            updateRoute(REMOVE_ROUTE, route );
            return true;
        }
    }
    
    //--------------------------------------------------------------
    void AdminConnection::updateRoute( RouteUpdateType type, Route route ){
        Json::Value message;
        message["route"] = Json::Value( Json::objectValue );
        message["route"]["publisher"]   = Json::Value( Json::objectValue );
        message["route"]["subscriber"]  = Json::Value( Json::objectValue );
        
        // the JS library checks to see if stuff exists before sending...
        // not doing that for now.
        switch (type) {
            case ADD_ROUTE:
                break;
            case REMOVE_ROUTE:
                break;
        }
        
        message["route"]["type"] = Json::Value(getRouteUpdateTypeString(type));
        
        // append pub + sub
        message["route"]["publisher"]["name"]           = route.getPublisher().name;
        message["route"]["publisher"]["type"]           = route.getPublisher().type;
        message["route"]["publisher"]["clientName"]     = route.getPublisher().clientName;
        message["route"]["publisher"]["remoteAddress"]  = route.getPublisher().remoteAddress;
        
        message["route"]["subscriber"]["name"]          = route.getSubscriber().name;
        message["route"]["subscriber"]["type"]          = route.getSubscriber().type;
        message["route"]["subscriber"]["clientName"]    = route.getSubscriber().clientName;
        message["route"]["subscriber"]["remoteAddress"] = route.getSubscriber().remoteAddress;
        
        // send to server
        if ( bConnected ){
        #ifdef SPACEBREW_USE_OFX_LWS
            client.send( message.toStyledString() );
        #endif
        } else {
            ofLog( OF_LOG_WARNING, "Send failed, not connected!");
        }
    }
    
    //--------------------------------------------------------------
    void AdminConnection::processIncomingJson( Json::Value & config ){
        // new connection
        if ( !config["config"].isNull() ){
            Config c;
            c.clientName    = config["config"]["name"].asString();
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
            
            // is this client us?
            // needs to be a better way to test this... basically just using this to add remoteAddress...
            if ( c.getJSON() == getConfig()->getJSON() ){
                getConfig()->remoteAddress = c.remoteAddress;
            }
            
            if ( bNew ){
                cout << "New?" << endl;
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
                    if ( connectedClients[j].clientName == name &&
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
            sub.clientName      = config["route"]["subscriber"]["clientName"].asString();
            sub.remoteAddress   = config["route"]["subscriber"]["remoteAddress"].asString();
            
            Route r( pub, sub );
            
            if ( config["route"]["type"].asString() == "add" ){
                currentRoutes.push_back(r);
                ofNotifyEvent(onRouteAdded, r, this);
            } else if ( config["route"]["type"].asString() == "remove"){
                for (int i=0; i<currentRoutes.size(); i++){
                    if ( currentRoutes[i] == r ){
                        ofNotifyEvent(onRouteRemoved, r, this);
                        currentRoutes.erase(currentRoutes.begin() + i );
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
    
    //--------------------------------------------------------------
    vector<Config> AdminConnection::getConnectedClients(){
        return connectedClients;
    }
    
    //--------------------------------------------------------------
    vector<Route> AdminConnection::getCurrentRoutes(){
        return currentRoutes;
    }
}