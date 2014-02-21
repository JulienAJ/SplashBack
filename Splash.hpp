class Splash {

	private:
		int** board;
		int columns;
		int lines;
		int shots;

	public:
		Splash();
		Splash(int, int, int); // L, C, S
		~Splash();
		void generate();

		void play();
		void display();
		bool empty();
		void action(int, int); // L, C
		void explode(int, int); // L, C
		void handle_action(int, int); //L, C
};
