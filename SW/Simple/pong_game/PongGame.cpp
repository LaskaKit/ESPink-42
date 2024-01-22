#include <GxEPD2_BW.h>

#define PADDLE_HEIGHT 40
#define PADDLE_WIDTH 10
#define BALL_RADIUS 5
#define BALL_SPEED 7
#define PADDLE_SPEED 7

static int16_t ball_x = 0, ball_y = 0;
static int8_t ball_dir_x = 1, ball_dir_y = 1;
static uint16_t player1_x = 0, player2_x = 0;

void PongGame_init(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display)
{
	player1_x = existing_display.width() - PADDLE_WIDTH - 1;
	player2_x = 1;
	existing_display.fillScreen(GxEPD_WHITE);
	existing_display.drawRect(0, 0, existing_display.width(), existing_display.height(), GxEPD_BLACK);

	ball_x = existing_display.width() / 2;
	ball_y = random((5 + BALL_RADIUS), (existing_display.height() - BALL_RADIUS - 5));
	if (random(0, 1))
	{
		ball_dir_x = BALL_SPEED;
	}
	else
	{
		ball_dir_x = -BALL_SPEED;
	}

	if (random(0, 1))
	{
		ball_dir_y = BALL_SPEED;
	}
	else
	{
		ball_dir_y = -BALL_SPEED;
	}
}

void PongGame(GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> &existing_display)
{
	static int16_t player2_y = (existing_display.height() / 2 - PADDLE_HEIGHT / 2);
	static int16_t player1_y = (existing_display.height() / 2 - PADDLE_HEIGHT / 2);
	int16_t new_x, new_y;

	new_x = ball_x + ball_dir_x;
	new_y = ball_y + ball_dir_y;

	// Check if we hit the horizontal walls.
	if (new_y <= (BALL_RADIUS))
	{
		ball_dir_y = -ball_dir_y;
		new_y = BALL_RADIUS + 1;
	}
	else if (new_y >= (existing_display.height() - (BALL_RADIUS)) - 1)
	{
		ball_dir_y = -ball_dir_y;
		new_y = existing_display.height() - BALL_RADIUS - 2;
	}

	// Check if we hit the  player 2 paddle
	if ((new_x <= (PADDLE_WIDTH + BALL_RADIUS)) && (new_y >= player2_y) && (new_y <= player2_y + PADDLE_HEIGHT))
	{
		ball_dir_x = -ball_dir_x;
		new_x = PADDLE_WIDTH + BALL_RADIUS + 1;
	}

	// Check if we hit the  player 1 paddle
	if ((new_x >= (existing_display.width() - PADDLE_WIDTH - BALL_RADIUS)) && (new_y >= player1_y) && (new_y <= player1_y + PADDLE_HEIGHT))
	{
		ball_dir_x = -ball_dir_x;
		new_x = existing_display.width() - PADDLE_WIDTH - BALL_RADIUS - 1;
	}

	// Draw ball
	existing_display.fillCircle(ball_x, ball_y, BALL_RADIUS, GxEPD_WHITE);
	existing_display.fillCircle(new_x, new_y, BALL_RADIUS, GxEPD_BLACK);
	ball_x = new_x;
	ball_y = new_y;

	// Player 2 paddle
	existing_display.fillRect(player2_x, player2_y, PADDLE_WIDTH, PADDLE_HEIGHT, GxEPD_WHITE);
	if ((ball_x < (existing_display.width() / 2)) && (ball_dir_x < 0))
	{
		if (ball_y > player2_y + PADDLE_HEIGHT / 2)
		{
			player2_y += PADDLE_SPEED;
		}
		else if (ball_y < player2_y + PADDLE_HEIGHT / 2)
		{
			player2_y -= PADDLE_SPEED;
		}
	}

	// Limit paddle 2 position
	if (player2_y < 2)
	{
		player2_y = 1;
	}

	if ((player2_y + PADDLE_HEIGHT - 1) > existing_display.height())
	{
		player2_y = existing_display.height() - PADDLE_HEIGHT - 1;
	}
	existing_display.fillRect(player2_x, player2_y, PADDLE_WIDTH, PADDLE_HEIGHT, GxEPD_BLACK);

	// Player 1 paddle
	existing_display.fillRect(player1_x, player1_y, PADDLE_WIDTH, PADDLE_HEIGHT, GxEPD_WHITE);
	if ((ball_x > (existing_display.width() / 2)) && (ball_dir_x > 0))
	{
		if (ball_y > player1_y + PADDLE_HEIGHT / 2)
		{
			player1_y += PADDLE_SPEED;
		}
		else if (ball_y < player1_y + PADDLE_HEIGHT / 2)
		{
			player1_y -= PADDLE_SPEED;
		}
	}

	// Limit paddle 1 position
	if (player1_y < 2)
	{
		player1_y = 1;
	}

	if ((player1_y + PADDLE_HEIGHT - 1) > existing_display.height())
	{
		player1_y = existing_display.height() - PADDLE_HEIGHT - 1;
	}
	existing_display.fillRect(player1_x, player1_y, PADDLE_WIDTH, PADDLE_HEIGHT, GxEPD_BLACK);
}