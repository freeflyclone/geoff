#ifndef APP_SESSION_H
#define APP_SESSION_H
#include "WebsockSessionManager.h"

// At present the GeoffLib WebsockServer::WebsockSessionManager needs to know about
// the derived-from-WebsockSession type being used for the particular
// application (game).  
// 
// Until a better way is invented,  this file serves to tell the 
// compiler how to complete the type definition for the desired 
// derived WebsockSession.  
// 
// It is expected that only a single "g_sessions" object will
// ever exist.  AsteroidsSession is just the first example.
#include "asteroids/inc/AsteroidsSession.h"
namespace Websock {
	extern WebsockSessionManager<AsteroidsSession> g_sessions;
};

#endif