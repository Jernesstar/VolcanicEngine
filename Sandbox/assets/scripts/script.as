
namespace UI {

class Button : IUIObject
{
    // Screen @thisScreen;

    Button()
    {
        // @thisScreen = screen;
    }

    void OnClick()
    {
        print("The button was clicked");
    }

    void OnHover()
    {
        print("Hovering...");
    }

    void OnMouseUp()
    {
        print("Mouse up");
    }

    void OnMouseDown()
    {
        print("Mouse down");
    }
}

}