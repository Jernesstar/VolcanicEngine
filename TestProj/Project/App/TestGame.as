
shared class GameState
{
	uint Level = 1;
	uint Coins = 50;
}

shared class TestGame : IApp
{
	GameState State;

	TestGame()
	{

	}

	void OnLoad()
	{
		print("OnLoad");
	}

	void OnClose()
	{
		print("OnClose");
	}

	void OnUpdate(float ts)
	{
		// print("OnUpdate: " + ts);
	}

	void NewMethod()
	{
		print("New Method");
	}
}
