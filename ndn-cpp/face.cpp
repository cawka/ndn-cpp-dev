/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "encoding/binary-xml-decoder.hpp"
#include "c/encoding/binary-xml.h"
#include "data.hpp"
#include "face.hpp"

using namespace std;
using namespace ndn::ptr_lib;

namespace ndn {

void Face::expressInterest(const Name &name, Closure *closure, const Interest *interestTemplate)
{
  Interest interest(name);
  shared_ptr<vector<unsigned char> > encoding = interest.wireEncode();  

  // TODO: This should go in the PIT.
  tempClosure_ = closure;
  
  transport_->connect(*this);
  transport_->send(*encoding);
}

void Face::processEvents()
{
  transport_->processEvents();
}

void Face::shutdown()
{
  transport_->close();
}

void Face::onReceivedElement(unsigned char *element, unsigned int elementLength)
{
  BinaryXmlDecoder decoder(element, elementLength);
  
  if (decoder.peekDTag(ndn_BinaryXml_DTag_ContentObject)) {
    shared_ptr<Data> data(new Data());
    data->wireDecode(element, elementLength);
    
    shared_ptr<Interest> dummyInterest;
    UpcallInfo upcallInfo(this, dummyInterest, 0, data);
    tempClosure_->upcall(UPCALL_DATA, upcallInfo);
  }
}

}