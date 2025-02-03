

class TestGame : IApp
{
	void OnLoad() {
		print("OnLoad");
	}

	void OnClose() {
		print("OnClose");
	}

	void OnUpdate(TimeStep ts) {
		print("OnUpdate");
	}
}
