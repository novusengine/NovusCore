#pragma once
/*
    MIT License

    Copyright (c) 2018-2019 NovusCore

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <Math/Vector3.h>
#include <vector>

#include "../Components/PlayerUpdateDataComponent.h"
#include "../../NovusEnums.h"

#define MAX_MOVEMENT_OPCODES 27
struct MovementData
{
    Vector3 position;
    u32 movementFlags = MOVEFLAG_NONE;
    u16 movementFlagsExtra = MOVEFLAG_EXTRA_NONE;
    u32 gameTime = 0;
    f32 orientation = 0;
    f32 pitch = 0;
    u32 fallTime = 0;

    // Transport Information
    Vector3 transportPosition;
    u64 transportGuid = 0;
    f32 transportOrientation = 0;
    u32 transportGameTime = 0;
    u32 transportInterpolatedGameTime = 0;
    u8 transportSeat = 0;

    // Falling / Knockback Information
    f32 zSpeed = 0;
    f32 sinusAngle = 0;
    f32 cosinusAngle = 0;
    f32 xySpeed = 0;

    // Spline Information
    f32 splineElevation = 0;
};

struct PlayerPositionComponent
{
    u16 mapId;
    MovementData movementData;

    u16 adtId = std::numeric_limits<u16>().max(); // Invalid ADT

    u32 initialServerTime = 0;
    u32 initialClientTime = 0;
    u32 lastMovementOpcodeTime[MAX_MOVEMENT_OPCODES];
    std::vector<PositionUpdateData> positionUpdateData;

    void WriteMovementData(std::shared_ptr<ByteBuffer> buffer, u32 gameTime)
    {
        // CDataStore *__cdecl CMovementStatus::Write(CDataStore *a1, int a2)

        buffer->PutU32(movementData.movementFlags);
        buffer->PutU16(movementData.movementFlagsExtra);
        buffer->PutU32(gameTime);
        buffer->Put<Vector3>(movementData.position);
        buffer->PutF32(movementData.orientation);

        if (movementData.movementFlags & MOVEFLAG_TRANSPORT)
        {
            buffer->PutGuid(movementData.transportGuid);
            buffer->Put<Vector3>(movementData.transportPosition);
            buffer->PutF32(movementData.transportOrientation);
            buffer->PutU32(movementData.transportGameTime);
            buffer->PutU8(movementData.transportSeat);

            if (movementData.movementFlagsExtra & MOVEFLAG_EXTRA_INTERPOLATED_MOVEMENT)
            {
                buffer->PutU32(movementData.transportInterpolatedGameTime);
            }
        }

        if (movementData.movementFlags & MOVEFLAG_MASK_PITCH || movementData.movementFlagsExtra & MOVEFLAG_EXTRA_ALWAYS_ALLOW_PITCHING)
        {
            buffer->PutF32(movementData.pitch);
        }

        buffer->PutU32(movementData.fallTime);

        if (movementData.movementFlags & MOVEFLAG_FALLING)
        {
            buffer->PutF32(movementData.zSpeed);
            buffer->PutF32(movementData.sinusAngle);
            buffer->PutF32(movementData.cosinusAngle);
            buffer->PutF32(movementData.xySpeed);
        }

        if (movementData.movementFlags & MOVEFLAG_SPLINE_ELEVATION)
        {
            buffer->PutF32(movementData.splineElevation);
        }
    }
    void WriteInitialMovementData(std::shared_ptr<ByteBuffer> buffer, u16 updateFlags, u32 gameTime)
    {
        buffer->PutU16(updateFlags);
        if (updateFlags & UPDATEFLAG_LIVING)
        {
            WriteMovementData(buffer, gameTime);

            // Movement Speeds
            buffer->PutF32(2.5f);      // MOVE_WALK
            buffer->PutF32(7.0f);      // MOVE_RUN
            buffer->PutF32(4.5f);      // MOVE_RUN_BACK
            buffer->PutF32(4.722222f); // MOVE_SWIM
            buffer->PutF32(2.5f);      // MOVE_SWIM_BACK
            buffer->PutF32(7.0f);      // MOVE_FLIGHT
            buffer->PutF32(4.5f);      // MOVE_FLIGHT_BACK
            buffer->PutF32(3.141593f); // MOVE_TURN_RATE
            buffer->PutF32(3.141593f); // MOVE_PITCH_RATE

            if (movementData.movementFlags & MOVEFLAG_SPLINE_ENABLED)
            {
                // We should write spline data here
            }
        }
        else
        {
            if (updateFlags & UPDATEFLAG_POSITION)
            {
            }
            else
            {
                if (updateFlags & UPDATEFLAG_STATIONARY_POSITION)
                {
                }
            }
        }

        if (updateFlags & UPDATEFLAG_UNK4)
        {
            buffer->PutU32(0);
        }

        if (updateFlags & UPDATEFLAG_LOWGUID)
        {
        }

        if (updateFlags & UPDATEFLAG_HAS_TARGET)
        {
        }

        if (updateFlags & UPDATEFLAG_TRANSPORT)
        {
        }

        if (updateFlags & UPDATEFLAG_VEHICLE)
        {
        }

        if (updateFlags & UPDATEFLAG_ROTATION)
        {

        }
    }
    void ReadMovementData(std::shared_ptr<ByteBuffer> buffer, MovementData& moveData)
    {
        // CDataStore *__cdecl CMovementStatus::Read(CDataStore *this, int arg4)

        buffer->GetU32(moveData.movementFlags);
        buffer->GetU16(moveData.movementFlagsExtra);
        buffer->GetU32(moveData.gameTime);
        buffer->Get<Vector3>(moveData.position);
        buffer->GetF32(moveData.orientation);

        if (moveData.movementFlags & MOVEFLAG_TRANSPORT)
        {
            buffer->GetGuid(moveData.transportGuid);
            buffer->Get<Vector3>(moveData.transportPosition);
            buffer->GetF32(moveData.transportOrientation);
            buffer->GetU32(moveData.transportGameTime);
            buffer->GetU8(moveData.transportSeat);

            if (moveData.movementFlagsExtra & MOVEFLAG_EXTRA_INTERPOLATED_MOVEMENT)
            {
                moveData.movementFlagsExtra &= 0xFBFFu;
                buffer->GetU32(moveData.transportInterpolatedGameTime);
            }
            else
            {
                moveData.transportInterpolatedGameTime = moveData.transportGameTime;
            }
        }

        if (moveData.movementFlags & MOVEFLAG_MASK_PITCH || moveData.movementFlagsExtra & MOVEFLAG_EXTRA_ALWAYS_ALLOW_PITCHING)
        {
            buffer->GetF32(moveData.pitch);
        }

        buffer->GetU32(moveData.fallTime);

        if (moveData.movementFlags & MOVEFLAG_FALLING)
        {
            buffer->GetF32(moveData.zSpeed);
            buffer->GetF32(moveData.sinusAngle);
            buffer->GetF32(moveData.cosinusAngle);
            buffer->GetF32(moveData.xySpeed);
        }

        if (moveData.movementFlags & MOVEFLAG_SPLINE_ELEVATION)
        {
            buffer->GetF32(moveData.splineElevation);
        }
    }
};