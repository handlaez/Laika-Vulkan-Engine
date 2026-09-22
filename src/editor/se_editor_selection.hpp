#ifndef SE_EDITOR_SELECTION_HPP
#define SE_EDITOR_SELECTION_HPP

#include "src/objects/le_actor.hpp"

#include <optional>

namespace se {

    class EditorSelection
    {
    public:
        void select(le::LeActor::id_t id)
        {
            selectedActor_ = id;
        }

        void clear()
        {
            selectedActor_.reset();
        }

        bool hasSelection() const
        {
            return selectedActor_.has_value();
        }

        std::optional<le::LeActor::id_t> getSelectedActor() const
        {
            return selectedActor_;
        }

    private:
        std::optional<le::LeActor::id_t> selectedActor_;
    };

}
#endif