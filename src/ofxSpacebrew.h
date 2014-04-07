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
    
    // Some useful constants
    static const int            SPACEBREW_PORT  = 9000;
    static const std::string    SPACEBREW_CLOUD = "sandbox.spacebrew.cc";
    static const std::string    TYPE_STRING     = "string";
    static const std::string    TYPE_RANGE      = "range";
    static const std::string    TYPE_BOOLEAN    = "boolean";
    
    /**
     * @brief Spacebrew message
     * @class Spacebrew::Message
     */
    class Message {
        public:
        
            /** @constructor */
            Message( string _name="", string _type="", string _val="");
            Message( string _name, string _type, string * _val );
            Message( string _name, string _type, int * _val );
            Message( string _name, string _type, bool * _val );
            Message( string _name, string _type, void * _val );
            virtual string getJSON( string configName );
            
            /**
             * @brief Name of Message
             * @type {std::string}
             */
            string name;

            /**
             * @brief Message type ("string", "boolean", "range", or custom type)
             * @type {std::string}
             */
            string type;

            /**
             * @brief Default value
             * @type {std::string}
             */
            string _default;

            /**
             * @brief Current value (cast to string)
             * @type {std::string}
             */
            string value;
        
            /**
             * @brief Get your incoming value as a boolean
             */
            bool    valueBoolean();
        
            /**
             * @brief Get your incoming value as a range (0-1023)
             */
            int     valueRange();
        
            /**
             * @brief Get your incoming value as a string
             */
            string    valueString();
        
            /**
             * @brief Update value (updates pointer if using reference)
             */
            void update( string value );
        
            // only compares name and type
            bool operator == ( Message & comp );
        
            friend ostream& operator<<(ostream& os, const Message& vec);
        
        protected:
            void * valuePtr;
            bool bUseReference;
    };
    
    inline ostream& operator<<(ostream& os, const Message& m) {
        os << m.name << ", " << m.type << ", " << m.value;
        return os;
    }
    
    /**
     * @brief Spacebrew binary message
     * @class Spacebrew::BinaryMessage
     */
    class BinaryMessage : public Spacebrew::Message
    {
    public:
        BinaryMessage( string _name, string _type, const ofBuffer buff ) : Message(_name, _type){
            buffer.clear();
            buffer.append(buff.getBinaryBuffer(), buff.size());
        }
        
        BinaryMessage( string _name, string _type, const char * data, const long size ) : Message(_name, _type){
            buffer.clear();
            buffer.append(data, size);
        }
        
        string getJSON( string configName ){
            stringstream ss;
            ss<<"{\"message\":{\"clientName\":\"";
            ss<<configName<<"\",\"name\":\""<<name + "\",";
            ss<<"\"type\":\"" + type + "\",";
            ss<<"\"value\":"<<buffer.size()<<"}}";
            return ss.str();
        }
        
        ofBuffer getSendBuffer( string configName ){
            outputBuffer.clear();
            string json = getJSON(configName);
            var jsonByteLength = json.length();
            int numBytesForJsonLength = (jsonByteLength > 0xFFFF ? 5 : (jsonByteLength >= 254 ? 3 : 1));
            if (numBytesForJsonLength == 5){
                outputBuffer.append(255, 1);
                outputBuffer.append([(char)(jsonByteLength >> 24), (char)(jsonByteLength >> 16), (char)(jsonByteLength >> 8), (char)(jsonByteLength)], 4);
            } else if (numBytesForJsonLength == 3){
                outputBuffer.append(254, 1);
                outputBuffer.append([(char)(jsonByteLength >> 8), (char)(jsonByteLength)], 2);
            } else {
                outputBuffer.append((char)jsonByteLength, 1);
            }
            outputBuffer.append( json );
            outputBuffer.append( buffer.getBinaryBuffer(), buffer.size() );
            return outputBuffer;
        }
        
        ofBuffer & data(){
            return buffer;
        }
        
    protected:
        ofBuffer buffer, outputBuffer;
    };
    
    /**
     * @brief Wrapper for Spacebrew config message. Gets created automatically by
     * Spacebrew::Connection, but can sometimes be nice to use yourself.
     * @class Spacebrew::Config
     */
    class Config {
        public:
            
            // see documentation below
            // docs left out here to avoid confusion. Most people will use these methods
            // on Spacebrew::Connection directly
            void addSubscribe( string name, string type );
            void addSubscribe( string name, string type, string * value );
            void addSubscribe( string name, string type, int * value );
            void addSubscribe( string name, string type, bool * value );
            void addSubscribe( string name, string type, void * value );
            void addSubscribe( Message m );
            void addPublish( string name, string type, string def);
            void addPublish( Message m );
        
            vector<Message> & getPublish();
            vector<Message> & getSubscribe();
        
            // remove all current publishers/subscribers (useful when updating)
            void resetPubSub();
        
            string getJSON();
            string clientName, description;
        
            //only used in configs from Admin Connection
            string remoteAddress;
        
            // note: only use this with Admin configs!
            // it only checks name/address
            bool operator == ( Config & comp );
        
        private:
            
            vector<Message> publish;
            vector<Message> subscribe;
    };
    
    /**
     * @brief Main Spacebrew class, connected to Spacebrew server. Sets up socket, builds configs
     * and publishes ofEvents on incoming messages.
     * @class Spacebrew::Connection
     */
    class Connection {
        public:
            Connection();
            ~Connection();
        
            /**
             * @brief Connect to Spacebrew. Pass empty values to connect to default host as "openFrameworks" app 
             * (use only for testing!)
             * @param {std::string} host        Host to connect to (e.g. "localhost", SPACEBREW_CLOUD ). Can be IP address OR hostname
             * @param {std::string} name        Name of your app (shows up in Spacebrew admin)
             * @param {std::string} description What does your app do?
             */
            void connect( string host = SPACEBREW_CLOUD, string name = "openFrameworks app", string description = "");
            void connect( string host, Config _config );
            
            /**
             * @brief Send a message
             * @param {std::string} name    Name of message
             * @param {std::string} type    Message type ("string", "boolean", "range", or custom type)
             * @param {std::string} value   Value (cast to string)
             */
            void send( string name, string type, string value );

            /**
             * @brief Send a string message
             * @param {std::string} name    Name of message
             * @param {std::string} value   Value
             */
            void sendString( string name, string value );

            /**
             * @brief Send a range message
             * @param {std::string} name    Name of message
             * @param {int}         value   Value
             */
            void sendRange( string name, int value );

            /**
             * @brief Send a boolean message
             * @param {std::string} name    Name of message
             * @param {bool}        value   Value
             */
            void sendBoolean( string name, bool value );

            /**
             * Send a Spacebrew Message object
             * @param {Spacebrew::Message} m
             */
            void send( Message m );

            /**
             * @brief Send a Spacebrew Message object. Use this method if you've overridden Spacebrew::Message
             * (especially) if you've created a custom getJson() method!)
             * @param {Spacebrew::Message} m
             */
            void send( Message * m );
        
            void sendBinary( string name, string type, ofBuffer data );
            void sendBinary( BinaryMessage m );
        
            /**
             * @brief Add a message that you want to subscribe to
             * @param {std::string} name    Name of message
             * @param {std::string} type    Message type ("string", "boolean", "range", or custom type)
             */
            void addSubscribe( string name, string type );

            /**
             * @brief Add a message that you want to subscribe to
             * @param {Spacebrew::Message} m
             */
            void addSubscribe( Message m );
        
            /**
             * @brief Automatically update a value when you receive a message
             * @param {std::string}     name    Name of message (assumes a "string" type)
             * @param {std::string *}   value   Pointer to string to update (will error and ignore call if not "string" type)
             */
            void addSubscribe( string name, string * value );
        
            /**
             * @brief Automatically update a value when you receive a message
             * @param {std::string}     name    Name of message (assumes a "boolean" type)
             * @param {bool *}          value   Pointer to bool to update (will error and ignore call if not "boolean" type)
             */
            void addSubscribe( string name, bool * value );
        
            /**
             * @brief Automatically update a value when you receive a message
             * @param {std::string}     name    Name of message (assumes a "range" type)
             * @param {std::string *}   value   Pointer to int to update
             */
            void addSubscribe( string name, int * value );

            // No subscribe hook for custom data yet...
        
            /**
             * @brief Add message of specific name + type to publish
             * @param {std::string} name Name of message
             * @param {std::string} typ  Message type ("string", "boolean", "range", or custom type)
             * @param {std::string} def  Default value
             */
            void addPublish( string name, string type, string def="");

            /**
             * @brief Add message to publish
             * @param {Spacebrew::Message} m
             */
            void addPublish( Message m );

            /**
             * @brief ofEvent to subscribe to!
             * @example ofAddListener(spacebrew.onMessageEvent, this, &testApp::onMessage);
             * void testApp::onMessage( Spacebrew::Message & m ){ 
             *     cout<< m.value << endl; 
             * };
             */
            ofEvent<Message> onMessageEvent;

            /**
             * @return Current Spacebrew::Config (list of publish/subscribe, etc)
             */
            Config * getConfig();
        
            /**
             * @return Are we connected?
             */
            bool isConnected();

            /**
             * @brief Turn on/off auto reconnect (try to connect when/if Spacebrew server closes)
             * @param {boolean} bAutoReconnect (true by default)
             */
            void setAutoReconnect( bool bAutoReconnect=true );

            /**
             * @brief How often should we try to reconnect if auto-reconnect is on (defaults to 1 second [1000 millis])
             * @param {int} reconnectMillis How often to reconnect, in milliseconds
             */
            void setReconnectRate( int reconnectMillis );

            /**
             * @return Are we trying to auto-reconnect?
             */
            bool doesAutoReconnect();

            /**
             * @return Current hostname
             */
            string getHost();
        
        #ifdef SPACEBREW_USE_OFX_LWS
            // websocket methods
            virtual void onConnect( ofxLibwebsockets::Event& args );
            virtual void onOpen( ofxLibwebsockets::Event& args );
            virtual void onClose( ofxLibwebsockets::Event& args );
            virtual void onIdle( ofxLibwebsockets::Event& args );
            virtual void onMessage( ofxLibwebsockets::Event& args );
            virtual void onBroadcast( ofxLibwebsockets::Event& args );
        
            ofEvent<ofxLibwebsockets::Event> onOpenEvent;
            ofEvent<ofxLibwebsockets::Event> onCloseEvent;
            ofxLibwebsockets::Client & getWebsocket();
        #else
        #endif
        
            // binary-only event
            ofEvent<BinaryMessage> onBinaryMessage;
        
        protected:
            void update( ofEventArgs & e );
        
            string host;
            bool bConnected;
            void updatePubSub();
        
            Config config;
            
            // reconnect
            bool bAutoReconnect;
            int  lastTimeTriedConnect;
            int  reconnectInterval;
        
            // binary send/receive: this is 100% dependent on your server!
            bool bSupportsBinary;
        
    #ifdef SPACEBREW_USE_OFX_LWS
            ofxLibwebsockets::Client client;
    #else
    #endif
    };
    
    /**
     * @brief Helper function to automatically add a listener to a connections onMessageEvent
     * @example 
     * Spacebrew::connection;
     * void onMessage( Message & e ){};
     *
     * void setup(){
     *      Spacebrew::addListener( this, connection);
     * }
     */
    template<class T, class SB>
    void addListener(T * app, SB & connection){
        ofAddListener( connection.onMessageEvent, app, &T::onMessage);
    }
        
    /**
     * @brief Helper function to remove onMessage listener
     */
    template<class T, class SB>
    void removeListener(T * app, SB & connection){
        ofRemoveListener( connection.onMessageEvent, app, &T::onMessage);
    }

    /**
     * @brief Simple struct that holds each end of a route.
     */
    class RouteEndpoint {
    public:
        string clientName;
        string name;
        string type;
        string remoteAddress;
        
        bool operator == (RouteEndpoint comp ){
            return comp.clientName == clientName && comp.name == name && comp.type == type && comp.remoteAddress == remoteAddress;
        }
    };
    
    enum RouteUpdateType {
        ADD_ROUTE,
        REMOVE_ROUTE
    };
        
    /**
     * @brief Util to translate from RouteUpdateType to its corresponding string in the Spacebrew API
     */
    static string getRouteUpdateTypeString( RouteUpdateType type ){
        switch( type){
            case ADD_ROUTE:
                return "add";
                break;
            case REMOVE_ROUTE:
                return "remove";
                break;
        }
    }
        
    /**
     * @class
     */
    class Route {
    public:
        
        Route(){};
        Route( RouteEndpoint pub, RouteEndpoint sub );
        
        void updatePublisher( RouteEndpoint pub );
        void updateSubscriber( RouteEndpoint pub );
        
        RouteEndpoint getPublisher();
        RouteEndpoint getSubscriber();
        
        inline bool operator == ( Route & r);
        
    private:
        RouteEndpoint publisher, subscriber;
    };

    /**
     * @brief Extends Message class, adding clientName and remoteAddress
     * @class Spacebrew::DataMessage
     */
    class DataMessage : public Message {
    public:
        string clientName;
        string remoteAddress;
    };

    /**
     * @brief Spacebrew Admin Connection: receives messages on new clients added,
     * clients removed, and routes added/removed
     * @extends Spacebrew::Connection
     * @class Spacebrew::AdminConnection
     */
    class AdminConnection : public Connection {
    public:
#ifdef SPACEBREW_USE_OFX_LWS
        // websocket methods
        virtual void onOpen( ofxLibwebsockets::Event& args );
        virtual void onMessage( ofxLibwebsockets::Event& args );
#else
#endif
        // add routes
        /**
         * Method that is used to add a route to the Spacebrew server
         * @param {String} pub_client               Publish client app name
         * @param {String} pub_address              Publish app remote IP address
         * @param {String} pub_name    				Publish name
         * @param {String} sub_client  				Subscribe client app name
         * @param {String} sub_address 				Subscribe app remote IP address
         * @param {String} sub_name    				Subscribe name
         * 
         * @memberOf Spacebrew::AdminConnection
         */
        bool addRoute( string pub_client, string pub_address, string pub_name,
                       string sub_client, string sub_address, string sub_name );
        
        /**
         * Method that is used to add a route to the Spacebrew server
         * @param {RouteEndpoint} pub_endpoint       Publisher endpoint
         * @param {RouteEndpoint} sub_endpoint       Subscriber endpoint
         *
         * @memberOf Spacebrew::AdminConnection
         */
        bool addRoute( RouteEndpoint pub_endpoint, RouteEndpoint sub_endpoint );
        
        /**
         * Method that is used to add a route to the Spacebrew server
         * @param {Route} route
         *
         * @memberOf Spacebrew::AdminConnection
         */
        bool addRoute( Route route );
        
        /**
         * Method that is used to remove a route from the Spacebrew server
         * @param {String} pub_client               Publish client app name
         * @param {String} pub_address              Publish app remote IP address
         * @param {String} pub_name    				Publish name
         * @param {String} sub_client  				Subscribe client app name
         * @param {String} sub_address 				Subscribe app remote IP address
         * @param {String} sub_name    				Subscribe name
         * 
         * @memberOf Spacebrew::AdminConnection
         */
        bool removeRoute( string pub_client, string pub_address, string pub_name,
                          string sub_client, string sub_address, string sub_name );
        
        /**
         * Method that is used to remove a route from the Spacebrew server
         * @param {RouteEndpoint} pub_endpoint       Publisher endpoint
         * @param {RouteEndpoint} sub_endpoint       Subscriber endpoint
         *
         * @memberOf Spacebrew::AdminConnection
         */
        bool removeRoute( RouteEndpoint pub_endpoint, RouteEndpoint sub_endpoint );
        
        /**
         * Method that is used to remove a route from the Spacebrew server
         * @param {Route} route                       
         *
         * @memberOf Spacebrew::AdminConnection
         */
        bool removeRoute( Route route );
        
        // events
        
        ofEvent<Config>     onClientConnectEvent;
        ofEvent<Config>     onClientUpdatedEvent;
        ofEvent<Config>     onClientDisconnectEvent;
        
        ofEvent<Route>      onRouteAdded;
        ofEvent<Route>      onRouteRemoved;
        
        ofEvent<DataMessage> onDataPublished;
        
        // getters
        vector<Config>      getConnectedClients();
        vector<Route>       getCurrentRoutes();
        
    protected:
        vector<Config>      connectedClients;
        vector<Route>       currentRoutes;
                
        /**
         * Method that handles both add and remove route requests. Responsible for parsing requests
         * and communicating with Spacebrew server
         */
        void updateRoute( RouteUpdateType type, Route route );
        
        void processIncomingJson( Json::Value & val );
    };
        
    /**
     * @brief Helper function to automatically add a listener to a admin connections events
     * Note: you must call the normal Spacebrew::addListener function to listen to normal messages
     * @example
     * Spacebrew::connection;
     *
     * void onClientConnect( Spacebrew::Config & e ){};
     * void onClientUpdated( Spacebrew::Config & e ){};
     * void onClientDisconnect( Spacebrew::Config & e ){};
     * void onRouteAdded( Spacebrew::Route & e ){};
     * void onRouteRemoved( Spacebrew::Route & e ){};
     * void onDataPublished( Spacebrew::DataMessage & e ){};
     *
     * void setup(){
     *      Spacebrew::addAdminListener( this, connection);
     * }
     */
    template<class T, class SB>
    void addAdminListener(T * app, SB & connection){
        ofAddListener( connection.onClientConnectEvent, app, &T::onClientConnect);
        ofAddListener( connection.onClientUpdatedEvent, app, &T::onClientUpdated);
        ofAddListener( connection.onClientDisconnectEvent, app, &T::onClientDisconnect);
        ofAddListener( connection.onRouteAdded, app, &T::onRouteAdded);
        ofAddListener( connection.onRouteRemoved, app, &T::onRouteRemoved);
        ofAddListener( connection.onDataPublished, app, &T::onDataPublished);
    }
    
    /**
     * @brief Helper function to remove onMessage listener
     */
    template<class T, class SB>
    void removeAdminListener(T * app, SB & connection){
        ofRemoveListener( connection.onClientConnectEvent, app, &T::onClientConnect);
        ofRemoveListener( connection.onClientUpdatedEvent, app, &T::onClientUpdated);
        ofRemoveListener( connection.onClientDisconnectEvent, app, &T::onClientDisconnect);
        ofRemoveListener( connection.onRouteAdded, app, &T::onRouteAdded);
        ofRemoveListener( connection.onRouteRemoved, app, &T::onRouteRemoved);
        ofRemoveListener( connection.onDataPublished, app, &T::onDataPublished);
    }
}
