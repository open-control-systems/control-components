/*
 * Copyright (c) 2024, Open Control Systems authors
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ocs_core/istream_writer.h"
#include "ocs_core/noncopyable.h"
#include "ocs_http/iresponse_writer.h"

namespace ocs {
namespace http {

//! Use chunked-encoding and send the response in the form of chunks.
class ChunkStreamWriter : public core::IStreamWriter, public core::NonCopyable<> {
public:
    //! Initialize.
    //!
    //! @params
    //!  - @p writer to write chunks of data.
    explicit ChunkStreamWriter(IResponseWriter& writer);

    //! Begin stream writing.
    status::StatusCode begin() override;

    //! End stream writing.
    status::StatusCode end() override;

    //! Cancel stream writing.
    status::StatusCode cancel() override;

    //! Write @p size bytes of @p data.
    status::StatusCode write(const void* data, unsigned size) override;

private:
    IResponseWriter& writer_;
};

} // namespace http
} // namespace ocs
