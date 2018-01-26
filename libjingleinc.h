#ifndef XMPPXEP_LIBJINBLEINC_H__
#define XMPPXEP_LIBJINBLEINC_H__

#include "libjingle/base/refcount.h"
#include "libjingle/base/scoped_ref_ptr.h"
#include "libjingle/base/scoped_ptr.h"
#include "libjingle/base/thread.h"
#include "libjingle/base/taskrunner.h"
#include "libjingle/base/asyncsocket.h"
#include "libjingle/base/bytebuffer.h"
#include "libjingle/base/stream.h"
#include "libjingle/base/jsoncpp/json.h"
#include "libjingle/base/json.h"
#include "libjingle/base/httpclient.h"
#include "libjingle/base/urlencode.h"
#include "libjingle/base/nethelpers.h"
#include "libjingle/base/fileutils.h"
#include "libjingle/base/pathutils.h"
#include "libjingle/base/win32.h"

/*
#include "libjingle/xmllite/xmlconstants.h"
#include "libjingle/xmllite/qname.h"
#include "libjingle/xmllite/xmlnsstack.h"
#include "libjingle/xmllite/xmlelement.h"
#include "libjingle/xmllite/xmlparser.h"
#include "libjingle/xmllite/xmlprinter.h"
#include "libjingle/xmllite/xmlbuilder.h"
#include "libjingle/xmllite/xmlrefbuilder.h"
*/



#include "AsyncFuncCall.h"
typedef talk_base::DisposeData< talk_base::IAsyncFuncCall >  IAsyncFuncCallDataPtr;




#endif