#ifndef __AMY_NOOP_DELETER_HPP_
#define __AMY_NOOP_DELETER_HPP_

namespace amy {

struct noop_deleter {
    void operator()(void*) {
        // no-op
    }

}; // struct noop_deleter

} // namespace amy

#endif /* __AMY_NOOP_DELETER_HPP_ */
