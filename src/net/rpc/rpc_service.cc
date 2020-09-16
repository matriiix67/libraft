/*
 * Copyright (C) lichuang
 */

#include "base/log.h"
#include "net/rpc/rpc_meta.h"
#include "net/rpc/rpc_service.h"

namespace libraft {
RpcService::RpcService(const ServiceOptions& options)
  : IService(options) {
  RpcServiceOptions& op = (RpcServiceOptions&)options;
  Register(op.service);
}

RpcService::~RpcService() {

}

void 
RpcService::Register(gpb::Service* service) {
  const gpb::ServiceDescriptor* serviceDescriptor =
      service->GetDescriptor();

  for (int i = 0; i < serviceDescriptor->method_count(); ++i) {
    const gpb::MethodDescriptor* methodDescriptor =
        serviceDescriptor->method(i);    
		uint64_t method_id = HashString(methodDescriptor->full_name());
		
    ASSERT(method_map_.find(method_id) == method_map_.end()) 
			<< "register duplicate method " << methodDescriptor->full_name();
    
    Info() << "register method " << methodDescriptor->full_name()
      << " with id " << method_id;

    RpcMeta* meta = new RpcMeta(service, methodDescriptor);
    method_map_[method_id] = meta;
  }
  //printMethod();
}

};