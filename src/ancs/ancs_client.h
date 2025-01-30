#ifndef ANCS_CLIENT_H
#define ANCS_CLIENT_H

#include "Task.h"

namespace ancs {

class ANCSClient: public Task {
    void run(void* data) override;
};

}

#endif // ANCS_CLIENT_H