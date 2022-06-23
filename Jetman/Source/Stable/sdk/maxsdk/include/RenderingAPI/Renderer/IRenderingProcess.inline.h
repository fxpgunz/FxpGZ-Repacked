//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2015 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

namespace MaxSDK
{;
namespace RenderingAPI
{;

inline IRenderingProcess::NamedTimerGuard::NamedTimerGuard(const MSTR& timer_name, IRenderingProcess& rendering_process, const bool pause)
    : m_pause(pause),
    m_timer_name(timer_name),
    m_rendering_process(rendering_process)
{
    if(m_pause)
    {
        // Pause
        m_rendering_process.StopTimer(m_timer_name);
    }
    else
    {
        // Start
        m_rendering_process.StartTimer(m_timer_name);
    }
}

inline IRenderingProcess::NamedTimerGuard::~NamedTimerGuard()
{
    if(m_pause)
    {
        // Resume
        m_rendering_process.StartTimer(m_timer_name);
    }
    else
    {
        // Stop
        m_rendering_process.StopTimer(m_timer_name);
    }
}

}}	// namespace 

