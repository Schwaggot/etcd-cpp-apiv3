#ifndef __ASYNC_LEASEACTION_HPP__
#define __ASYNC_LEASEACTION_HPP__

#include <mutex>

#include <grpc++/grpc++.h>
#include "proto/rpc.grpc.pb.h"
#include "etcd/v3/Action.hpp"
#include "etcd/v3/AsyncLeaseResponse.hpp"
#include "etcd/Response.hpp"

using grpc::ClientAsyncResponseReader;
using grpc::ClientAsyncReaderWriter;
using etcdserverpb::LeaseGrantResponse;
using etcdserverpb::LeaseRevokeResponse;
using etcdserverpb::LeaseCheckpoint;
using etcdserverpb::LeaseCheckpointResponse;
using etcdserverpb::LeaseKeepAliveRequest;
using etcdserverpb::LeaseKeepAliveResponse;
using etcdserverpb::LeaseTimeToLiveResponse;
using etcdserverpb::LeaseStatus;
using etcdserverpb::LeaseLeasesResponse;

namespace etcd {
  class KeepAlive;
}

namespace etcdv3
{
  class AsyncLeaseGrantAction : public etcdv3::Action {
    public:
      AsyncLeaseGrantAction(etcdv3::ActionParameters && params);
      AsyncLeaseGrantResponse ParseResponse();
    private:
      LeaseGrantResponse reply;
      std::unique_ptr<ClientAsyncResponseReader<LeaseGrantResponse>> response_reader;
  };

  class AsyncLeaseRevokeAction: public etcdv3::Action {
    public:
      AsyncLeaseRevokeAction(etcdv3::ActionParameters && params);
      AsyncLeaseRevokeResponse ParseResponse();
    private:
      LeaseRevokeResponse reply;
      std::unique_ptr<ClientAsyncResponseReader<LeaseRevokeResponse>> response_reader;
  };

  class AsyncLeaseKeepAliveAction: public etcdv3::Action {
    public:
      AsyncLeaseKeepAliveAction(etcdv3::ActionParameters && params);
      AsyncLeaseKeepAliveResponse ParseResponse();

      etcd::Response Refresh();
      void CancelKeepAlive();
      bool Cancelled() const;

    private:
      etcdv3::ActionParameters& mutable_parameters();

      LeaseKeepAliveResponse reply;
      std::unique_ptr<ClientAsyncReaderWriter<LeaseKeepAliveRequest, LeaseKeepAliveResponse>> stream;

      LeaseKeepAliveRequest req;
      bool isCancelled;
      std::recursive_mutex protect_is_cancelled;

      friend class etcd::KeepAlive;
  };

  class AsyncLeaseTimeToLiveAction: public etcdv3::Action {
    public:
      AsyncLeaseTimeToLiveAction(etcdv3::ActionParameters && params);
      AsyncLeaseTimeToLiveResponse ParseResponse();
    private:
      LeaseTimeToLiveResponse reply;
      std::unique_ptr<ClientAsyncResponseReader<LeaseTimeToLiveResponse>> response_reader;
  };

  class AsyncLeaseLeasesAction: public etcdv3::Action {
    public:
      AsyncLeaseLeasesAction(etcdv3::ActionParameters && params);
      AsyncLeaseLeasesResponse ParseResponse();
    private:
      LeaseLeasesResponse reply;
      std::unique_ptr<ClientAsyncResponseReader<LeaseLeasesResponse>> response_reader;
  };
}

#endif
