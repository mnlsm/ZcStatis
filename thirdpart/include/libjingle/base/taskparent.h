/*
 * libjingle
 * Copyright 2004--2006, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TALK_BASE_TASKPARENT_H__
#define TALK_BASE_TASKPARENT_H__

#include <set>

#include "libjingle/base/basictypes.h"
#include "libjingle/base/scoped_ptr.h"

namespace talk_base
{

class Task;
class TaskRunner;

class TaskParent
{
public:
    TaskParent( Task *derived_instance, TaskParent *parent );
    explicit TaskParent( TaskRunner *derived_instance );
    virtual ~TaskParent();

public:
    TaskParent *GetParent();
    TaskRunner *GetRunner();
    bool AllChildrenDone();
    bool AnyChildError();

#ifdef _DEBUG
    bool IsChildTask( Task *task );
#endif

protected:
    void OnStopped( Task *task );
    void AbortAllChildren();
    TaskParent *parent();

private:
    void Initialize();
    void OnChildStopped( Task *child );
    void AddChild( Task *child );

private:
    TaskParent *parent_;
    TaskRunner *runner_;
    bool child_error_;
    typedef std::set<Task *> ChildSet;
    scoped_ptr<ChildSet> children_;
    DISALLOW_EVIL_CONSTRUCTORS( TaskParent );
};


} // namespace talk_base

#endif  // TALK_BASE_TASKPARENT_H__
