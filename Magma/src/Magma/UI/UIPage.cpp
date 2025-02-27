#include "UIPage.h"

#include "UI.h"

#include <VolcaniCore/Core/Log.h>

namespace Magma::UI {

static void TraverseElement(UIElement* element,
							const Func<void, UIElement*>& func)
{
	func(element);
	for(UIElement* child : element->GetChildren())
		TraverseElement(child, func);
}

static void TraverseElement(UIElement* element,
							const Func<void, UIElement*, TraversalStage>& func)
{
	func(element, TraversalStage::Begin);
	for(UIElement* child : element->GetChildren())
		TraverseElement(child, func);
	func(element, TraversalStage::End);
}

UIPage& UIPage::operator=(const UIPage& other) {
	FirstOrders = other.FirstOrders;
	Windows = other.Windows;
	Buttons = other.Buttons;
	Dropdowns = other.Dropdowns;
	Texts = other.Texts;
	TextInputs = other.TextInputs;
	Images = other.Images;

	for(auto& val : Windows)
		val.m_Root = this;
	for(auto& val : Buttons)
		val.m_Root = this;
	for(auto& val : Dropdowns)
		val.m_Root = this;
	for(auto& val : Texts)
		val.m_Root = this;
	for(auto& val : TextInputs)
		val.m_Root = this;
	for(auto& val : Images)
		val.m_Root = this;

	return *this;
}

void UIPage::Render() {
	for(UIElement* element : GetFirstOrderElements())
		element->Render();

	UIRenderer::Pop(0);
}

void UIPage::Traverse(const Func<void, UIElement*>& func, bool dfs) {
	if(dfs) {
		for(UIElement* element : GetFirstOrderElements())
			TraverseElement(element, func);
		return;
	}

	List<UIElement*> q;
	for(UIElement* element : GetFirstOrderElements())
		q.Add(element);

	while(q) {
		UIElement* element = q.Pop(-1);
		func(element);

		for(UIElement* child : element->GetChildren())
			q.Queue(child);
	}
}

void UIPage::Traverse(const Func<void, UIElement*, TraversalStage>& func) {
	for(UIElement* element : GetFirstOrderElements())
		TraverseElement(element, func);
}

UINode UIPage::Add(UIElementType type, const std::string& id) {
	switch(type) {
		case UIElementType::Window:
		{
			auto& element = Windows.Emplace(id, this);
			element.SetNode({ type, Windows.Count() - 1 });
			return element.GetNode();
			break;
		}
		case UIElementType::Button:
		{
			auto& element = Buttons.Emplace(id, this);
			element.SetNode({ type, Buttons.Count() - 1 });
			return element.GetNode();
			break;
		}
		case UIElementType::Dropdown:
		{
			auto& element = Dropdowns.Emplace(id, this);
			element.SetNode({ type, Dropdowns.Count() - 1 });
			return element.GetNode();
			break;
		}
		case UIElementType::Text:
		{
			auto& element = Texts.Emplace(id, this);
			element.SetNode({ type, Texts.Count() - 1 });
			return element.GetNode();
			break;
		}
		case UIElementType::TextInput:
		{
			auto& element = TextInputs.Emplace(id, this);
			element.SetNode({ type, TextInputs.Count() - 1 });
			return element.GetNode();
			break;
		}
		case UIElementType::Image:
		{
			auto& element = Images.Emplace(id, this);
			element.SetNode({ type, Images.Count() - 1 });
			return element.GetNode();
			break;
		}
	}

	return { UIElementType::None, 0 };
}

void UIPage::Add(const UINode& node) {
	FirstOrders.Add(node);
}

void UIPage::Clear() {
	FirstOrders.Clear();
	Windows.Clear();
	Buttons.Clear();
	Dropdowns.Clear();
	Texts.Clear();
	TextInputs.Clear();
	Images.Clear();
}

UIElement* UIPage::Get(const UINode& node) const {
	switch(node.first) {
		case UIElementType::Window:
		{
			if(node.second < Windows.Count())
				return dynamic_cast<UIElement*>(Windows.At(node.second));
			break;
		}
		case UIElementType::Button:
		{
			if(node.second < Buttons.Count())
				return dynamic_cast<UIElement*>(Buttons.At(node.second));
			break;
		}
		case UIElementType::Dropdown:
		{
			if(node.second < Dropdowns.Count())
				return dynamic_cast<UIElement*>(Dropdowns.At(node.second));
			break;
		}
		case UIElementType::Text:
		{
			if(node.second < Texts.Count())
				return dynamic_cast<UIElement*>(Texts.At(node.second));
			break;
		}
		case UIElementType::TextInput:
		{
			if(node.second < TextInputs.Count())
				return dynamic_cast<UIElement*>(TextInputs.At(node.second));
			break;
		}
		case UIElementType::Image:
		{
			if(node.second < Images.Count())
				return dynamic_cast<UIElement*>(Images.At(node.second));
			break;
		}
	}

	return nullptr;
}

UIElement* UIPage::Get(const std::string& id) const {
	if(auto res = Windows.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Windows.At(res.Index));
	if(auto res = Buttons.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Buttons.At(res.Index));
	if(auto res = Dropdowns.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Dropdowns.At(res.Index));
	if(auto res = Texts.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Texts.At(res.Index));
	if(auto res = TextInputs.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(TextInputs.At(res.Index));
	if(auto res = Images.Find([&](auto& val) { return val.GetID() == id; }))
		return dynamic_cast<UIElement*>(Images.At(res.Index));

	return nullptr;
}

List<UIElement*> UIPage::GetFirstOrderElements() const {
	List<UIElement*> res;
	for(auto node : FirstOrders)
		res.Add(Get(node));

	return res;
}

}