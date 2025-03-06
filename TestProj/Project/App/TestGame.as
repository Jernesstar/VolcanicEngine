
class TestGame : IApp
{
	void OnLoad()
	{
		print("OnLoad");
		App.SetScreen("This is a test");
	}

	void OnClose()
	{
		print("OnClose");
	}

	void OnUpdate(float ts)
	{
		// print("OnUpdate: " + ts);
	}
}
