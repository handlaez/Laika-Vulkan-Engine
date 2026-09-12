#ifndef I_EDITOR_PANEL_HPP
#define I_EDITOR_PANEL_HPP

namespace se {
	class IEditorPanel {
	public:
		virtual ~IEditorPanel() = default;

		virtual void onImGuiRender() = 0;
		virtual void onUpdate() {};
	};
}

#endif