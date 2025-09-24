#pragma once
namespace chtljs {

enum class State {
    Init,
    Tokenize,
    Parse,
    BindVirtual,
    BuildKeyMap,
    Transform,
    Generate,
    Done,
    Error
};

}

