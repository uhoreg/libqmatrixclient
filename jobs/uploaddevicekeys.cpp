/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *******************************************************************************/

#include "uploaddevicekeys.h"

#include "../encryptionmanager.h"
#include "converters.h"

using namespace QMatrixClient;

UploadDeviceKeys::UploadDeviceKeys(ConnectionData* connection,
                                   EncryptionManager* encryptionManager)
    : BaseJob(connection, HttpVerb::Post, "UploadDeviceKeys"
            , "_matrix/client/unstable/keys/upload"
            , Query()
            , Data(
                { { "device_id", encryptionManager->deviceId() }
                , { "algorithms", toJson(QStringList {
                    "m.olm.v1.curve25519-aes-sha2", "m.megolm.v1.aes-sha2" }) }
                , { "keys", encryptionManager->publicIdentityKeys() }
                , { "user_id", encryptionManager->userId() }
            })
        )
    , em(encryptionManager)
{ }
