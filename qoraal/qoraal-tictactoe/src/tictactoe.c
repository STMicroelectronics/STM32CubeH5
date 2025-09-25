/******************************************************************************
 * Tic Tac Toe with Q-Learning and Neural Network
 *
 * This file implements a Tic Tac Toe game with AI. It is split into several
 * sections:
 *   1. Includes: Standard and project-specific headers.
 *   2. Global Definitions & Macros: Constants for game states, cell states,
 *      and shell command declarations.
 *   3. Engine Constants & Actions: Implements game actions (restart, play,
 *      status query, cell query) for the embedded engine.
 *   4. Neural Network for AI: Implements a simple neural network and 
 *      Q-learning training functions to choose moves (Courtesy ChatGPT o3).
 *   5. Shell Commands: Commands to run a demonstration game and training loop
 *      from the console.
 *
 * Enjoy the mix of hardcore AI and classic Tic Tac Toe. If you’re not here for
 * the real deal, step aside!
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"

/*
 * Section: Global Function Declarations & Shell Command Declarations
 *
 * These static functions provide move selection, winner checking, and board
 * fullness testing. Additionally, shell command declarations tie commands to
 * their corresponding functions.
 */
static int choose_move_epsilon(int board[9], double epsilon);
static int check_winner(int board[9], int win[9]);
static int board_full(int board[9]);

SVC_SHELL_CMD_DECL("tictactoe", qshell_cmd_tictactoe, "");
SVC_SHELL_CMD_DECL("tictactrain", qshell_cmd_tictactrain, "[iterations]");

/*
 * Section: Global Macros & Variables
 *
 * Defines constants for game state and cell status. Global arrays hold the
 * board state and blinking cells, while _tictactoe_player tracks whose turn.
 */

#define DBG_MESSAGE_TICTACTOE(severity, fmt_str, ...)     DBG_MESSAGE_T(SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)


#define TICTACTTOE_STATE_PLAY               0
#define TICTACTTOE_STATE_DRAW               1
#define TICTACTTOE_STATE_PLAYER_WIN         2
#define TICTACTTOE_STATE_AI_WIN             3

#define TICTACTTOE_CELL_OPEN                0
#define TICTACTTOE_CELL_PLAYER              1
#define TICTACTTOE_CELL_AI                  2
#define TICTACTTOE_CELL_PLAYER_BLINK        3
#define TICTACTTOE_CELL_AI_BLINK            4

int _tictactoe_board[9] = {0,0,0, 0,0,0, 0,0,0};
int _tictactoe_board_blink[9] = {0,0,0, 0,0,0, 0,0,0};
int _tictactoe_player = TICTACTTOE_CELL_PLAYER;

/*
 * Section: Engine Constants & Action Implementations
 *
 * These macros and functions integrate with the engine framework. They
 * provide the constants (with ENGINE_CONST_IMPL) and actions (with
 * ENGINE_ACTION_IMPL) used to control game flow.
 */
ENGINE_CONST_IMPL(TICTACTTOE_STATE_PLAY,        TICTAC_PLAY,         "Game in play");
ENGINE_CONST_IMPL(TICTACTTOE_STATE_DRAW,        TICTAC_DRAW,         "Draw");
ENGINE_CONST_IMPL(TICTACTTOE_STATE_PLAYER_WIN,  TICTAC_PLAYER_WIN,   "Player win");
ENGINE_CONST_IMPL(TICTACTTOE_STATE_AI_WIN,      TICTAC_AI_WIN,       "AI win");
ENGINE_CONST_IMPL(TICTACTTOE_CELL_OPEN,         TICTAC_OPEN,         "Cell open");
ENGINE_CONST_IMPL(TICTACTTOE_CELL_PLAYER,       TICTAC_PLAYER,       "Cell used by player");
ENGINE_CONST_IMPL(TICTACTTOE_CELL_AI,           TICTAC_AI,           "Cell used by AI");
ENGINE_CONST_IMPL(TICTACTTOE_CELL_PLAYER_BLINK, TICTAC_PLAYER_BLINK, "Cell used by player");
ENGINE_CONST_IMPL(TICTACTTOE_CELL_AI_BLINK,     TICTAC_AI_BLINK,     "Cell used by AI");

ENGINE_ACTION_IMPL(tictac_restart, "Restart the game");
ENGINE_ACTION_IMPL(tictac_play,    "User play the cell number 0-8 in the param");
ENGINE_ACTION_IMPL(tictac_status,  "Returns the TICTA_xxx state of the game");
ENGINE_ACTION_IMPL(tictac_cell,    "Returns the CELL_xxx state for cell param 0-8");

/**
 * @brief Restarts the Tic Tac Toe game.
 *
 * This action resets the board and alternates the starting player.
 * If the starting player becomes AI, the AI immediately makes a move.
 */
int32_t action_tictac_restart(PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return EOK;
    }

    DBG_MESSAGE_TICTACTOE (DBG_MESSAGE_SEVERITY_REPORT, 
                "TIC   : : New game starting with %s.",
                _tictactoe_player == TICTACTTOE_CELL_PLAYER ? "Player" : "AI") ;

    memset(_tictactoe_board, 0, sizeof(_tictactoe_board));
    memset(_tictactoe_board_blink, 0, sizeof(_tictactoe_board_blink));
    if (_tictactoe_player == TICTACTTOE_CELL_PLAYER) {
        _tictactoe_player = TICTACTTOE_CELL_AI;
        return EOK;
    }
    _tictactoe_player = TICTACTTOE_CELL_PLAYER;

    int action = choose_move_epsilon(_tictactoe_board, 0.0);
    if (action < 0)
        return EFAIL;

    _tictactoe_board[action] = 1;
    _tictactoe_board_blink[action] = 1;

    return EOK;
}

/**
 * @brief Processes a player's move.
 *
 * Validates the move, updates the board, checks for a win, and if the game
 * is still on, lets the AI choose its move.
 */
int32_t action_tictac_play(PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return EOK;
    }
    memset(_tictactoe_board_blink, 0, sizeof(_tictactoe_board_blink));
    if (parm < 0 || parm >= 9)
        return EFAIL;
    if (_tictactoe_board[parm])
        return EFAIL;
    if (check_winner(_tictactoe_board, _tictactoe_board_blink))
        return EOK;
    if (board_full(_tictactoe_board))
        return EOK ;

    _tictactoe_board[parm] = -1;
    if (check_winner(_tictactoe_board, _tictactoe_board_blink)) {
            DBG_MESSAGE_TICTACTOE (DBG_MESSAGE_SEVERITY_REPORT, 
                "TIC   : : Player wins!") ;
        return EOK;
    }
    if (board_full(_tictactoe_board)) {
            DBG_MESSAGE_TICTACTOE (DBG_MESSAGE_SEVERITY_REPORT, 
                "TIC   : : Draw") ;
        return EOK ;

    }


    int action = choose_move_epsilon(_tictactoe_board, 0.0);
    if (action < 0)
        return EFAIL;

    _tictactoe_board[action] = 1;
    _tictactoe_board_blink[action] = 1;

    if (check_winner(_tictactoe_board, _tictactoe_board_blink)) {
            DBG_MESSAGE_TICTACTOE (DBG_MESSAGE_SEVERITY_REPORT, 
                "TIC   : : AI wins!") ;

    }
    if (board_full(_tictactoe_board)) {
            DBG_MESSAGE_TICTACTOE (DBG_MESSAGE_SEVERITY_REPORT, 
                "TIC   : : Draw") ;

    }


    return EOK;
}

/**
 * @brief Returns the current game status.
 *
 * Determines if the AI or player has won, if the game is a draw,
 * or if the game is still in progress.
 */
int32_t action_tictac_status(PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return EOK;
    }

    int winner = check_winner(_tictactoe_board, _tictactoe_board_blink);
    if (winner == 1)
        return TICTACTTOE_STATE_AI_WIN;
    else if (winner == -1)
        return TICTACTTOE_STATE_PLAYER_WIN;
    else if (board_full(_tictactoe_board))
        return TICTACTTOE_STATE_DRAW;

    return TICTACTTOE_STATE_PLAY;
}

/**
 * @brief Retrieves the status of a single cell on the board.
 *
 * Returns one of the CELL_xxx constants for a given board position.
 */
int32_t action_tictac_cell(PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return EOK;
    }
    if (parm < 0 || parm >= 9)
        return EFAIL;

    if (_tictactoe_board[parm] == -1)
        return _tictactoe_board_blink[parm] ? TICTACTTOE_CELL_PLAYER_BLINK : TICTACTTOE_CELL_PLAYER;
    if (_tictactoe_board[parm] == 1)
        return _tictactoe_board_blink[parm] ? TICTACTTOE_CELL_AI_BLINK : TICTACTTOE_CELL_AI;

    return TICTACTTOE_CELL_OPEN;
}

/*
 * Section: Neural Network for Tic Tac Toe AI (Courtesy ChatGPT o3)
 *
 * This section defines the neural network used for the AI's move selection.
 * It includes:
 *   - Network structure and initialization.
 *   - Forward propagation routines.
 *   - Q-learning training step.
 *   - Utility functions to convert board state into network input.
 */

#define INPUT_SIZE 9
#define HIDDEN_SIZE 9
#define OUTPUT_SIZE 9

typedef struct {
    double weights_input_hidden[HIDDEN_SIZE][INPUT_SIZE];
    double hidden_biases[HIDDEN_SIZE];
    double weights_hidden_output[OUTPUT_SIZE][HIDDEN_SIZE];
    double output_biases[OUTPUT_SIZE];
} NeuralNet;

static NeuralNet _tictactoe_net = {
    // Pre-initialized weights for input-to-hidden layer (can be overridden by training)
    {
        { 0.106844, 2.297165, -4.215321, 0.590149, -4.113728, 1.123057, -2.406111, 0.652693, 2.410074 },
        { -1.485419, 2.351545, -3.254365, -0.902239, 1.054924, -0.451825, -0.939223, 2.492350, -2.197246 },
        { 0.721562, 0.868623, 2.571977, -3.532937, 0.216167, -3.186169, -2.161942, -4.266016, -1.760417 },
        { 4.915549, 0.471722, -2.216907, 3.307417, -1.075988, 0.506404, 1.420466, 0.498970, -2.845725 },
        { -0.707013, -0.628700, -0.923353, 1.123700, 5.734343, 1.747154, 0.590114, 2.511602, 1.158106 },
        { 1.471237, 0.480728, -3.428099, 3.401812, -1.676710, 3.400047, -1.495710, 3.318829, 2.706812 },
        { 3.480505, 3.960648, 3.179807, -2.151266, 1.626941, -1.576451, -0.142268, 0.399096, -0.765087 },
        { -6.733041, -5.435084, 5.806235, -0.590666, 0.693655, -0.774133, -0.212025, -0.534461, -0.619377 },
        { 5.663660, 1.372808, 0.289790, -0.783382, 4.128768, -2.408603, 0.482060, 0.022660, -4.602510 }
    },
    // Pre-initialized biases for hidden layer
    { -1.085286, 2.790615, 3.996572, -0.227859, -5.441354, -0.241621, -3.390699, -0.275622, 2.635717 },
    // Pre-initialized weights for hidden-to-output layer
    {
        { 0.736400, 2.059622, 6.050683, -0.965378, -1.117015, -0.062528, -4.617711, -1.298687, 2.435539 },
        { -0.588383, 2.293386, 4.156750, -0.474892, -2.906036, 0.603409, -3.668664, -3.515782, -0.010221 },
        { 1.241191, 2.734489, 4.463278, -0.015362, -3.074955, -0.441200, -1.653899, 7.363835, -0.865544 },
        { 1.127200, -0.962272, -0.273045, 0.690156, -3.969938, -0.305883, 2.244259, -1.192854, -0.043179 },
        { 3.799981, -2.281462, 4.464080, 3.658008, 2.005016, -5.011692, -1.505154, 0.082717, 0.789965 },
        { 1.657756, -2.761479, 0.867956, -0.471131, -2.479380, 0.702748, 3.613657, -1.765321, -1.952891 },
        { 5.723476, 0.786463, 4.733712, 0.615118, -2.254674, -5.613541, -4.248687, 0.032763, 1.037714 },
        { 0.913864, -3.744873, -0.529591, 3.051642, -4.441503, 0.459809, -2.606837, 0.049611, -1.037329 },
        { 1.230620, 0.645857, 3.983960, 4.192639, -0.019080, 0.526311, 2.018243, 0.517314, -8.125889 }
    },
    // Pre-initialized biases for output layer
    { -0.893318, 1.636943, -0.055843, -1.541960, 0.008355, -0.923598, 2.041277, 0.620558, -0.677074 }
};

/**
 * @brief Sigmoid activation function.
 *
 * @param x Input value.
 * @return Sigmoid of x.
 */
static double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

/**
 * @brief Generates a random weight between -1 and 1.
 *
 * @return A random double value.
 */
static double rand_weight() {
    return ((double)rand() / RAND_MAX) * 2.0 - 1.0;
}

/**
 * @brief Initializes the neural network with random weights.
 *
 * This overrides the pre-initialized weights. Use this if you want to train
 * from scratch.
 */
static void init_network(void) {
    NeuralNet *net = &_tictactoe_net;
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            net->weights_input_hidden[i][j] = rand_weight();
        }
        net->hidden_biases[i] = rand_weight();
    }
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            net->weights_hidden_output[i][j] = rand_weight();
        }
        net->output_biases[i] = rand_weight();
    }
}

/**
 * @brief Performs forward propagation through the network.
 *
 * Computes the activations for the hidden and output layers based on the given
 * input.
 *
 * @param input  Input vector (board state).
 * @param hidden Output vector for hidden layer.
 * @param output Output vector for output layer.
 */
static void forward_with_hidden(double input[INPUT_SIZE], double hidden[HIDDEN_SIZE], double output[OUTPUT_SIZE]) {
    NeuralNet *net = &_tictactoe_net;
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        double sum = net->hidden_biases[i];
        for (int j = 0; j < INPUT_SIZE; j++) {
            sum += net->weights_input_hidden[i][j] * input[j];
        }
        hidden[i] = sigmoid(sum);
    }
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        double sum = net->output_biases[i];
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            sum += net->weights_hidden_output[i][j] * hidden[j];
        }
        output[i] = sigmoid(sum);
    }
}

/**
 * @brief Converts board state to neural network input.
 *
 * Maps board cell values: -1 to 0.0, 0 to 0.5, and 1 to 1.0.
 *
 * @param board The game board.
 * @param input The resulting input vector.
 */
static void board_to_input(int board[9], double input[9]) {
    for (int i = 0; i < 9; i++) {
        input[i] = (board[i] + 1) / 2.0;
    }
}

/**
 * @brief Checks if there is a winner on the board.
 *
 * Evaluates all win conditions. If a win is detected, optionally marks the
 * winning cells in the provided win array.
 *
 * @param board The game board.
 * @param win   Array to mark winning cells (can be NULL).
 * @return 1 if player 1 wins, -1 if player -1 wins, 0 otherwise.
 */
static int check_winner(int board[9], int win[9]) {
    int wins[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
    };
    for (int i = 0; i < 8; i++) {
        int a = wins[i][0], b = wins[i][1], c = wins[i][2];
        if (board[a] != 0 && board[a] == board[b] && board[b] == board[c]) {
            if (win)
                win[a] = win[b] = win[c] = board[a];
            return board[a];
        }
    }
    return 0;
}

/**
 * @brief Checks if the board is full.
 *
 * @param board The game board.
 * @return 1 if full, 0 otherwise.
 */
static int board_full(int board[9]) {
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0)
            return 0;
    }
    return 1;
}

/**
 * @brief Chooses a move using an epsilon-greedy strategy.
 *
 * With probability epsilon, a random legal move is chosen (exploration);
 * otherwise, the move with the highest network output is selected (exploitation).
 *
 * @param board   The game board.
 * @param epsilon Exploration rate.
 * @return The chosen move index, or -1 if no legal moves exist.
 */
static int choose_move_epsilon(int board[9], double epsilon) {
    NeuralNet *net = &_tictactoe_net;
    int legal_moves[9], count = 0;
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0)
            legal_moves[count++] = i;
    }
    if (count == 0)
        return -1;  // No moves available

    double r = (double)rand() / RAND_MAX;
    if (r < epsilon) {
        // Explore: choose a random legal move
        return legal_moves[rand() % count];
    } else {
        // Exploit: select move with highest output value
        double input[INPUT_SIZE], hidden[HIDDEN_SIZE], output[OUTPUT_SIZE];
        board_to_input(board, input);
        forward_with_hidden(input, hidden, output);
        int best_move = legal_moves[0];
        double best_value = -1.0;
        for (int i = 0; i < count; i++) {
            int move = legal_moves[i];
            if (output[move] > best_value) {
                best_value = output[move];
                best_move = move;
            }
        }
        return best_move;
    }
}

/**
 * @brief Executes one Q-learning training step.
 *
 * Updates the network weights for the action taken based on the reward received
 * and the expected future rewards.
 *
 * @param board        Current board state.
 * @param action       Action taken.
 * @param reward       Reward received.
 * @param next_board   Board state after the action.
 * @param terminal     1 if next_board is terminal, 0 otherwise.
 * @param learning_rate Learning rate for weight updates.
 * @param gamma        Discount factor.
 */
static void train_step(int board[9], int action, double reward, int next_board[9], int terminal, double learning_rate, double gamma) {
    NeuralNet *net = &_tictactoe_net;
    double input[INPUT_SIZE];
    board_to_input(board, input);
    double hidden[HIDDEN_SIZE], output[OUTPUT_SIZE];
    forward_with_hidden(input, hidden, output);

    double q_sa = output[action];
    double target;
    if (terminal) {
        target = reward;
    } else {
        double next_input[INPUT_SIZE], next_hidden[HIDDEN_SIZE], next_output[OUTPUT_SIZE];
        board_to_input(next_board, next_input);
        forward_with_hidden(next_input, next_hidden, next_output);
        double max_next = 0.0;
        int first = 1;
        for (int i = 0; i < 9; i++) {
            if (next_board[i] == 0) {
                if (first) {
                    max_next = next_output[i];
                    first = 0;
                } else if (next_output[i] > max_next) {
                    max_next = next_output[i];
                }
            }
        }
        target = reward + gamma * max_next;
    }

    double error = target - q_sa;
    double output_deriv = q_sa * (1 - q_sa);
    double delta_output = error * output_deriv;

    // Update weights for the chosen action in hidden-to-output layer.
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        net->weights_hidden_output[action][i] += learning_rate * delta_output * hidden[i];
    }
    net->output_biases[action] += learning_rate * delta_output;

    // Backpropagate into hidden layer.
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        double error_hidden = delta_output * net->weights_hidden_output[action][i];
        double hidden_deriv = hidden[i] * (1 - hidden[i]);
        double delta_hidden = error_hidden * hidden_deriv;
        for (int j = 0; j < INPUT_SIZE; j++) {
            net->weights_input_hidden[i][j] += learning_rate * delta_hidden * input[j];
        }
        net->hidden_biases[i] += learning_rate * delta_hidden;
    }
}

/**
 * @brief Copies the board state from src to dest.
 */
static void copy_board(int dest[9], int src[9]) {
    memcpy(dest, src, sizeof(int) * 9);
}

/**
 * @brief Prints the Tic Tac Toe board.
 *
 * Converts board values to characters: -1 to 'O', 0 to ' ', 1 to 'X'.
 *
 * @param pif   Shell interface for output.
 * @param board The game board.
 */
static void print_board(SVC_SHELL_IF_T *pif, int board[9]) {
    char symbols[3] = { 'X', ' ', 'O' };
    for (int i = 0; i < 9; i++) {
        int idx = board[i] + 1;  // Adjust index: -1 -> 0, 0 -> 1, 1 -> 2
        svc_shell_print(pif, SVC_SHELL_OUT_STD, " %c ", symbols[idx]);
        if ((i + 1) % 3 == 0) {
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "\n");
            if (i != 8)
                svc_shell_print(pif, SVC_SHELL_OUT_STD, "---+---+---\n");
        } else {
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "|");
        }
    }
}

/**
 * @brief Prints the current neural network weights as C initialization arrays.
 *
 * Useful for exporting the trained network.
 *
 * @param pif Shell interface for output.
 */
static void print_trained_weights(SVC_SHELL_IF_T *pif) {
    NeuralNet *net = &_tictactoe_net;
    int i, j;
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "\n// Trained Weights for Initialization\n\n");

    // Print weights_input_hidden.
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "double weights_input_hidden[%d][%d] = {\n", HIDDEN_SIZE, INPUT_SIZE);
    for (i = 0; i < HIDDEN_SIZE; i++) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "    { ");
        for (j = 0; j < INPUT_SIZE; j++) {
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "%.6f", net->weights_input_hidden[i][j]);
            if (j < INPUT_SIZE - 1)
                svc_shell_print(pif, SVC_SHELL_OUT_STD, ", ");
        }
        svc_shell_print(pif, SVC_SHELL_OUT_STD, " }");
        if (i < HIDDEN_SIZE - 1)
            svc_shell_print(pif, SVC_SHELL_OUT_STD, ",\n");
        else
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "\n");
    }
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "};\n\n");

    // Print hidden_biases.
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "double hidden_biases[%d] = { ", HIDDEN_SIZE);
    for (i = 0; i < HIDDEN_SIZE; i++) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "%.6f", net->hidden_biases[i]);
        if (i < HIDDEN_SIZE - 1)
            svc_shell_print(pif, SVC_SHELL_OUT_STD, ", ");
    }
    svc_shell_print(pif, SVC_SHELL_OUT_STD, " };\n\n");

    // Print weights_hidden_output.
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "double weights_hidden_output[%d][%d] = {\n", OUTPUT_SIZE, HIDDEN_SIZE);
    for (i = 0; i < OUTPUT_SIZE; i++) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "    { ");
        for (j = 0; j < HIDDEN_SIZE; j++) {
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "%.6f", net->weights_hidden_output[i][j]);
            if (j < HIDDEN_SIZE - 1)
                svc_shell_print(pif, SVC_SHELL_OUT_STD, ", ");
        }
        svc_shell_print(pif, SVC_SHELL_OUT_STD, " }");
        if (i < OUTPUT_SIZE - 1)
            svc_shell_print(pif, SVC_SHELL_OUT_STD, ",\n");
        else
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "\n");
    }
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "};\n\n");

    // Print output_biases.
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "double output_biases[%d] = { ", OUTPUT_SIZE);
    for (i = 0; i < OUTPUT_SIZE; i++) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "%.6f", net->output_biases[i]);
        if (i < OUTPUT_SIZE - 1)
            svc_shell_print(pif, SVC_SHELL_OUT_STD, ", ");
    }
    svc_shell_print(pif, SVC_SHELL_OUT_STD, " };\n\n");
}

/*
 * Section: Shell Commands for Demonstration and Training
 *
 * These commands allow you to run a demonstration game (AI vs. random opponent)
 * and to train the AI using Q-learning.
 */

/**
 * @brief Shell command to run a demonstration game.
 *
 * The game plays out between the AI and a random opponent. The board is printed
 * after each move and the result is announced at the end.
 */
static int32_t qshell_cmd_tictactoe(SVC_SHELL_IF_T *pif, char** argv, int argc)
{
    svc_shell_print(pif, SVC_SHELL_OUT_STD, "\n=== Demonstration Game ===\n");
    int board[9] = {0,0,0, 0,0,0, 0,0,0};
    int current_player = 0;
    while (!check_winner(board, 0) && !board_full(board)) {
        if (current_player == 1) {
            int action = choose_move_epsilon(board, 0.0);
            if (action == -1)
                break;
            board[action] = 1;
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "\nAI moves at position %d:\n", action);
            print_board(pif, board);
            current_player = -1;
        } else {
            int legal_moves[9], count = 0;
            for (int i = 0; i < 9; i++) {
                if (board[i] == 0)
                    legal_moves[count++] = i;
            }
            if (count == 0)
                break;
            int move = legal_moves[rand() % count];
            board[move] = -1;
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "\nOpponent moves at position %d:\n", move);
            print_board(pif, board);
            current_player = 1;
        }
    }
    int winner = check_winner(board, 0);
    if (winner == 1)
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "\nAI wins the demonstration game!\n");
    else if (winner == -1)
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "\nOpponent wins the demonstration game!\n");
    else
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "\nIt's a draw in the demonstration game!\n");

    return EOK;
}

/**
 * @brief Shell command to train the AI.
 *
 * Runs a training loop where the AI plays against a random opponent, updating its
 * neural network using Q-learning. The training parameters (episodes, learning rate,
 * discount factor, exploration rate) are defined and can be adjusted.
 */
static int32_t qshell_cmd_tictactrain(SVC_SHELL_IF_T *pif, char** argv, int argc)
{
    srand(time(NULL));
    init_network();

    // Training hyperparameters.
    int episodes = 200000;
    double learning_rate = 0.1;
    double gamma = 0.9;
    double epsilon = 0.3;  // initial exploration rate

    if (argc > 1) {
        sscanf(argv[1], "%u", &episodes);
    }

    // Training loop: AI plays against a random opponent.
    for (int ep = 0; ep < episodes; ep++) {
        int ai_player = (rand() % 2 == 0) ? 1 : -1;
        int board[9] = {0,0,0, 0,0,0, 0,0,0};
        int game_over = 0;
        int current_player = 1;  // game starts with player 1
        int last_ai_board[9];
        int last_ai_action = -1;
        int ai_waiting_update = 0;

        while (!game_over) {
            if (current_player == ai_player) {
                // AI's turn.
                copy_board(last_ai_board, board);
                int action = choose_move_epsilon(board, epsilon);
                if (action == -1)
                    break;
                board[action] = ai_player;

                int winner = check_winner(board, 0);
                if (winner == ai_player) {
                    // AI wins: reward +1.
                    train_step(last_ai_board, action, 1.0, board, 1, learning_rate, gamma);
                    game_over = 1;
                } else if (board_full(board)) {
                    // Draw: reward 0.5.
                    train_step(last_ai_board, action, 0.5, board, 1, learning_rate, gamma);
                    game_over = 1;
                } else {
                    last_ai_action = action;
                    ai_waiting_update = 1;
                    current_player = -current_player;
                }
            } else {
                // Opponent's turn (random move).
                int legal_moves[9], count = 0;
                for (int i = 0; i < 9; i++) {
                    if (board[i] == 0)
                        legal_moves[count++] = i;
                }
                if (count == 0)
                    break;
                int move = legal_moves[rand() % count];
                board[move] = -ai_player; // Opponent uses the opposite marker.

                int winner = check_winner(board, 0);
                if (winner == -ai_player) {
                    // Opponent wins: AI loses (reward -1).
                    if (ai_waiting_update)
                        train_step(last_ai_board, last_ai_action, -1.0, board, 1, learning_rate, gamma);
                    game_over = 1;
                } else if (board_full(board)) {
                    // Draw: reward 0.5.
                    if (ai_waiting_update)
                        train_step(last_ai_board, last_ai_action, 0.5, board, 1, learning_rate, gamma);
                    game_over = 1;
                } else {
                    if (ai_waiting_update) {
                        train_step(last_ai_board, last_ai_action, 0.0, board, 0, learning_rate, gamma);
                        ai_waiting_update = 0;
                    }
                    current_player = -current_player;
                }
            }
        }
        // Decay epsilon gradually.
        if (epsilon > 0.05)
            epsilon *= 0.9999;
        if ((ep + 1) % 5000 == 0)
            svc_shell_print(pif, SVC_SHELL_OUT_STD, "Episode %d completed. Epsilon: %.3f\n", ep + 1, epsilon);
    }

    // Output the trained weights for future initialization.
    print_trained_weights(pif);
    return 0;
}