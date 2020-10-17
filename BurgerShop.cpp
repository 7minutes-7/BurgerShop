#include <bangtal>
#include <iostream>

using namespace bangtal;
using namespace std;

bool playing = false;

void reset(ObjectPtr burger, ObjectPtr next, ScenePtr scene, int* type, int* location, int* pass) {
	if (burger->isHanded()) { burger->drop(); }
	burger->show();
	burger->setImage("Images/empty.png");
	burger->locate(scene, 62, 200); next->locate(scene, 85, 100);
	*type = 0; *location = 0; *pass = 0;
}

class Customer {
private:
	TimerPtr customerIn_timer = Timer::create(0.005f);
	TimerPtr customerOut_timer = Timer::create(0.005f);
	TimerPtr pickup_timer = Timer::create(1.0f);
	TimerPtr price_timer = Timer::create(1.0f);

	int answers[5][5] = { {10012111,11110011,10000111,10111101,10100011},{10000101,11011111,10111111,11112111,10001101},{11111111,11111011,11102101,10000111,11101111},{10002001,11112111,11110001,11111011,11001101},{11111011,11102111,11000011,10000001,10102101} };//다섯 손님, 각 세문제,각 정답 최대 세개씩
	string orders[5][5] = { {"참깨빵 위에 순쇠고기 패티 두장\n치즈, 피클, 양파까지~","오늘부터 베지테리언이 되기로 했어요","요즘 치피빵이 유행이라면서요?","초록색 극.혐.","이디야는 토피넛라떼\n햄버거는 토피버거! 고기 빼는 건 알죠?"},
		{"빵집이 닫았어요. 치즈빵 주세요","엄마는 토마토가 싫다고 하셨어요","엄마가 어른은 야채도 먹는거래요\n그치만 양상추는 정말 싫어요","오늘 아빠가 용돈 줬어요 부럽죠!\n제일 비싼 걸로 주세요!!","치즈버거 주세욥"},
		{"고기 추가하면 돈 내야 해?\n어휴 사기꾼들이야.....안 낼 거야!!! ","1000원 꽉꽉 채워서 줘!!!!!\n그 노란 거는 넣기만 해 봐!!!!!!","그 뭐냐 피크린가 뭔가는 빼고, 그 보라색 그것도 빼고,\n그래 기분이다! 고기 추가해서 하나 줘봐","요즘 애들이 치피빵인가 뭔가 많이 먹더라?","양파가 몸에 좋아 알어???\n너 많이 먹어야하니까 난 빼고 줘"},
		{"온리 고기만 가득. 근손실 올 거 같으니까 빨리요;;","여자친구가 헤어지재요...그냥 다 넣어서 주세요\n고기 추가도..욖끄흡...끆ㄱ끄얶륶끕.." ,"횐님을 위한 야채버거 부탁드려요\n아 피클을 빼달래요","저 유당불내증이에요","토마토 빼고, 양파 빼고, 피클 빼고\n오늘은 하체 조질거니까 맛있게 해줘요"},
		{"엄..노 치즈..앤 노 토마토..\n쏘리 예스 토마토 플리즈","어니언 입냄새 노우...쏘 투 패티 플리즈 땡큐","마이 페이보릿 밴드 이즈 그린데이\n쏘 투데이 이즈 그린 버거 데이","두 유 해브 버터? 노?\n 덴 저스트 브레드 플리즈","토마토 앤 치즈 이즈 베스트 유노?\n 토마토 앤 치즈 플러스 투 패티 플리즈"} };
	ObjectPtr customers[5];
	string reaction[5][2] = { {"고마워요!","여기 다시는 오나 봐라;;"},{"고마워요 아저씨!","엄마한테 이를 거야..."},{"흠 맛있네","장난해? 우리집 뽀삐가 이것보단 잘 만들겠다"},{"득근하십쇼~!!","닭가슴살이나 먹을게요"},{"와우 엑셀런트 음뫄","fXck"} };

	SoundPtr customer1_order = Sound::create("Sounds/customer1.wav");
	SoundPtr customer2_order = Sound::create("Sounds/customer2.wav");
	SoundPtr customer3_order = Sound::create("Sounds/customer3.wav");
	SoundPtr customer4_order = Sound::create("Sounds/customer4.wav");
	SoundPtr customer5_order = Sound::create("Sounds/customer5.wav");

	SoundPtr customer1_happy = Sound::create("Sounds/customer1_happy.wav");
	SoundPtr customer2_happy = Sound::create("Sounds/customer2_happy.wav");
	SoundPtr customer3_happy = Sound::create("Sounds/customer3_happy.wav");
	SoundPtr customer4_happy = Sound::create("Sounds/customer4_happy.wav");
	SoundPtr customer5_happy = Sound::create("Sounds/customer5_happy.wav");

	SoundPtr customer1_mad = Sound::create("Sounds/customer1_mad.wav");
	SoundPtr customer2_mad = Sound::create("Sounds/customer2_mad.wav");
	SoundPtr customer3_mad = Sound::create("Sounds/customer3_mad.wav");
	SoundPtr customer4_mad = Sound::create("Sounds/customer4_mad.wav");
	SoundPtr customer5_mad = Sound::create("Sounds/customer5_mad.wav");

	SoundPtr reaction_sounds[5][3] = { {customer1_order,customer1_happy,customer1_mad},{customer2_order,customer2_happy,customer2_mad} ,{customer3_order,customer3_happy,customer3_mad} ,{customer4_order,customer4_happy,customer4_mad} ,{customer5_order,customer5_happy,customer5_mad} };
	SoundID givesMoney = createSound("Sounds/cha_ching.mp3");
	SoundID customerEnter = createSound("Sounds/door_opening.mp3");
	SoundID pickupBell = createSound("Sounds/pickup_bell.mp3");
public:
	Customer(ScenePtr scene, bool shown) { //손님 생성
		for (int i = 0; i < 5; i++) {
			string filename = "Images/customer" + to_string(i + 1) + ".png";
			customers[i] = Object::create(filename, scene, 640, 720);
			cout << filename << endl;
		}
	}
	bool served = false;
	int customer_num, customer_order = 0;
	float happy_customer = 0;
	int customer_x = 640, customer_y = 720;

	ObjectPtr getCustomer(int i) { return customers[i]; }
public:
	void customerIn(int num, ScenePtr scene) {//손님 들어옴
		customerIn_timer->setOnTimerCallback([=](TimerPtr t)->bool {
			customers[customer_num]->locate(scene, customer_x, customer_y);
			customer_y -= 5;
			if (customer_y > 0) {
				t->set(0.005f);
				t->start();
			}
			else {
				showMessage(orders[customer_num][customer_order]);
				reaction_sounds[customer_num][0]->play();
			}
			t->set(0.005f);
			return true;
			});
		playSound(customerEnter);
		customerIn_timer->start();
		customer_num = num;
		customer_order = rand() % 5; //손님 주문 랜덤
	}
	void customerOut(int num, ScenePtr scene) {//손님 나감
		customerOut_timer->setOnTimerCallback([=](TimerPtr t)->bool {
			customers[num]->locate(scene, customer_x, customer_y);
			customer_y += 4;
			if (customer_y < 720) {
				t->set(0.005f);
				t->start();
			}
			else {
				if (playing) {
					string filename = "Images/customer" + to_string(num + 1) + ".png";
					getCustomer(num)->setImage(filename);
					int customer_num = rand() % 5; //손님 랜덤
					customerIn(customer_num, scene);
				}
			}
			return true;
			});
		customerOut_timer->start();
	}

public:
	void handBurger(ScenePtr counter, ScenePtr kitchen, ObjectPtr customer, int customer_type, int order_type, ObjectPtr burger, ObjectPtr price, int* type, int* location, int* pass, ObjectPtr next) {
		customer->setOnMouseCallback([=](ObjectPtr object, int x, int y, MouseAction)->bool {
			if (burger->isHanded()) {
				playSound(pickupBell);
				burger->drop();
				burger->locate(counter, 900, 100);
				pickup_timer->setOnTimerCallback([&](TimerPtr t)->bool {
					string filename;
					if (answers[customer_num][customer_order] == *type) { //주문 맞음
						filename = "Images/customer" + to_string(customer_num + 1) + "_happy.png";
						customer->setImage(filename);
						showMessage(reaction[customer_num][0]);
						reaction_sounds[customer_num][1]->play();
						burger->hide();
						served = true;
						happy_customer++;
						if ((*type % 10000) / 1000 == 2) {  //고기 추가하면 500원 추가
							happy_customer += 0.5;
						}
						price->show(); playSound(givesMoney);
						price_timer->setOnTimerCallback([&](TimerPtr t)->bool {
							price->hide();
							t->set(1.0f);
							return true;
							});
						price_timer->start();
					}
					else {
						filename = "Images/customer" + to_string(customer_num + 1) + "_mad.png";
						customer->setImage(filename);
						showMessage(reaction[customer_num][1]);   //주문 틀림
						reaction_sounds[customer_num][2]->play();
						burger->hide();
						served = true;
					}
					if (served) {
						customerOut(customer_num, counter);
						pickup_timer->set(1.0f);
						reset(burger, next, kitchen, type, location, pass);
					}
					return true;
					});
				pickup_timer->start();
			}
			return true;
			});
	}
};

class Food {
private:
	TimerPtr topping_timer = Timer::create(0.0505f);
	SceneID make_burger = createSound("Sounds/make_burger.mp3");
public:
	ObjectPtr toppings[8];
	int type, location, pass = 0;
	int burger_x = 62, burger_y = 200, diff = 150;

	Food(ScenePtr scene) { //토핑 버튼 생성
		for (int i = 0; i < 8; i++) {
			toppings[i] = Object::create("Images/button.png", scene, 72 + diff * i, 450);
		}
	}
public:
	void setBurger(ObjectPtr burger, string food) { //버거 이미지 변경
		playSound(make_burger);
		if (food == "patty" && type / 10000 == 1) {
			type = type + 1;
			string filename = "Images/" + food + to_string(type) + ".png";
			burger->setImage(filename);
		}
		else {
			type = type * 10 + 1;
			string filename = "Images/" + food + to_string(type) + ".png";
			burger->setImage(filename);
		}
	}
};

int main() {
	auto mii_theme = createSound("Sounds/mii_theme.mp3"); 	playSound(mii_theme, true);
	auto play_music = createSound("Sounds/play_music.mp3");
	auto tada = createSound("Sounds/tada.mp3");
	auto trashSound = createSound("Sounds/trash.mp3");

	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	TimerPtr icon_timer = Timer::create(0.2f); bool lighted = true;
	TimerPtr game_timer = Timer::create(120.0f); //2분 안에 돈을 얼마나 벌 수 있나
	TimerPtr burger_timer = Timer::create(1.0f);

	auto title = Scene::create("", "Images/title.png");
	auto counter = Scene::create("", "Images/blank.png");
	auto kitchen = Scene::create("", "Images/kitchen.png");
	auto result = Scene::create("", "Images/blank.png");

	Customer customer(counter, true);
	auto table = Object::create("Images/table.png", counter, 0, 0);
	auto menu = Object::create("Images/menu.png", counter, 200, 450);
	auto me = Object::create("Images/me.png", counter, 70, 0);
	auto trash = Object::create("Images/trash.png", kitchen, 1000, 10);

	auto play_image = Object::create("Images/play.png", title, 500, 500);
	auto play = Object::create("Images/playbutton.png", title, 500, 500);
	auto kitchen_icon = Object::create("Images/kitchen_icon.png", counter, 40, 600);
	auto register_icon = Object::create("Images/register_icon.png", kitchen, 40, 600);
	auto again = Object::create("Images/again.png", result, 100, 360);
	auto exit = Object::create("Images/exit.png", result, 650, 360);

	auto regular = Object::create("Images/1000.png", counter, 900, 300, false);
	auto upgrade = Object::create("Images/1500.png", counter, 900, 300, false);

	int highscore = 0;
	int money = 0;

	icon_timer->setOnTimerCallback([&](TimerPtr t)->bool {
		if (lighted) { play_image->hide(); lighted = false; }
		else { play_image->show(); lighted = true; }
		t->set(0.2f);
		t->start();
		return true;
		});
	icon_timer->start();

	kitchen_icon->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		kitchen->enter();
		return true;
		});
	register_icon->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		counter->enter();
		return true;
		});

	game_timer->setOnTimerCallback([&](TimerPtr t)->bool {
		result->setOnEnterCallback([&](ScenePtr scene)->bool {
			playing = false; customer.customer_x = 640; customer.customer_y = 720;
			setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
			setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
			hideTimer();
			stopSound(play_music);
			playSound(mii_theme, true);
			playSound(tada);

			money = customer.happy_customer * 1000;
			if (money > highscore) { highscore = money; }
			string buf = "오늘의 매출은 " + to_string(money) + "원입니다! \n 당신의 최고기록은 " + to_string(highscore) + "원입니다";
			showMessage(buf.c_str());
			return true;
			});

		result->enter();
		return true;
		});

	string topping_names[8] = { "bottom_bun", "lettuce", "tomatoes", "onions", "patty", "cheese", "pickles", "top_bun" };
	Food food(kitchen);
	int next_x = 85, next_y = 100;
	auto next = Object::create("Images/next.png", kitchen, next_x, next_y);
	auto burger = Object::create("Images/empty.png", kitchen, food.burger_x, food.burger_y);


	//다음 재료로 넘어가기
	next->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		if (food.type > 0 && food.location < 7) {
			food.location++; food.pass++;
			next->locate(kitchen, next_x + food.diff * food.location, next_y);
			burger->locate(kitchen, food.burger_x + food.diff * food.location, food.burger_y);
			if (food.pass >= 2) food.type = food.type * 10;
		}
		return true;
		});

	//재료 쌓기
	for (int i = 0; i < 8; i++) {
		food.toppings[i]->setOnMouseCallback([&, i](ObjectPtr object, int x, int y, MouseAction action)->bool {
			if (food.type == 0 && food.location == i) { food.setBurger(burger, topping_names[0]); }
			else if (i > 0 && food.type / pow(10, i - 1) < 10 && food.location == i) { food.setBurger(burger, topping_names[i]); food.pass = 0; }
			else if (i == 4 && food.type % 2 == 1 && food.location == 4) { food.setBurger(burger, topping_names[4]); food.pass = 0; }
			if (i == 7)burger->locate(kitchen, food.burger_x + food.diff * food.location, food.burger_y);
			return true;
			});
	}
	//다 만든 버거 줍기
	burger->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		if (food.type / 10000000 == 1) burger->pick();

		//손님한테 버거 전달	
		for (int i = 0; i < 5; i++) {
			if ((food.type % 10000) / 1000 == 2) { customer.handBurger(counter, kitchen, customer.getCustomer(i), 0, 0, burger, upgrade, &food.type, &food.location, &food.pass, next); }
			else { customer.handBurger(counter, kitchen, customer.getCustomer(i), 0, 0, burger, regular, &food.type, &food.location, &food.pass, next); }
		}
		return true;
		});

	//잘못 만든 버거 버릴 수 있게 (새로 시작)
	trash->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		playSound(trashSound);
		reset(burger, next, kitchen, &food.type, &food.location, &food.pass);
		return true;
		});

	//다시 시작
	again->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		title->enter();
		game_timer->set(60.0f);
		reset(burger, next, kitchen, &food.type, &food.location, &food.pass);
		customer.getCustomer(customer.customer_num)->locate(counter, 640, 720);
		customer.customer_x = 640, customer.customer_y = 720;
		customer.happy_customer = 0;
		return true;
		});
	//게임 시작
	play->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		playing = true;
		setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, true);
		setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, true);
		counter->enter();
		showTimer(game_timer);
		game_timer->start();
		stopSound(mii_theme);
		playSound(play_music, true);

		int customer_num = rand() % 5; //손님 입장
		customer.customerIn(customer_num, counter);
		return true;
		});

	//게임 종료
	exit->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
		endGame();
		return true;
		});

	startGame(title);
	return 0;
}