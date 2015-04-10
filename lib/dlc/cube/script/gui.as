class GUIController : IController
{
    GameObject self;

    GUIController(GameObject& in ptr)
    {
        self = ptr;
    }

    void onGUI()
    {
        GUI.drawLine(-0.02, -0.0, 0.02, 0.0, 1, float4(0.1, 0.1, 0.1, 1.0));
        GUI.drawLine(0, -0.02, 0, 0.02, 1, float4(0.1, 0.1, 0.1, 1.0));
    }
}