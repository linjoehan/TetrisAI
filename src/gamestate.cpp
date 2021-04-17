#include "Gamestate.h"

Move::Move()
{
    int rotation = 0;
    int column = 0;
}

Move::Move(int rotation,int column)
{
    this->rotation = rotation;
    this->column = column;
}

bool Move::operator==(Move &test)
{
    return rotation == test.rotation and column == test.column;
}

bool Move::operator!=(Move &test)
{
    return not(*this == test);
}

Block::Block()
{
    Block("            ####");
}

Block::Block(std::string input)
{
    for(int row = 0;row<4;row++)
    {
        for(int col = 0;col<4;col++)
        {
            cell[row][col] = input[row*4 + col];
        }
    }
}

void Block::print()
{
    for(int row = 0;row<4;row++)
    {
        for(int col = 0;col<4;col++)
        {
            std::cout << cell[row][col];
        }
        std::cout << std::endl;
    }
}

bool Block::operator==(Block &test)
{
    bool res = true;
    for(int row = 0;row<4;row++)
    {
        for(int col = 0;col<4;col++)
        {
            if(cell[row][col] != test.cell[row][col])
            {
                res = false;
            }
        }
    }
    return res;
}

Gamestate::Gamestate()
{
    Gamestate(0);
}

Gamestate::Gamestate(int start_level)
: base_lines{0,10,20,30,40,50,60,70,80,90,90,90,90,90,90,90,100,110,120,130},
  lines{0,0,0,0,0},
  line_score{0,40,100,300,1200},
  score(0)
{
    this->start_level = start_level;
    game_over = false;
    srand(time(0));
    
    for(int row = 0;row<25;row++)
    {
        for(int col = 0;col<10;col++)
        {
            board[row][col] = ' ';
        }
    }
    
    //I piece
    blocks[0][0] = Block("            ####");
    blocks[0][1] = Block("#   #   #   #   ");
    blocks[0][2] = Block("            ####");
    blocks[0][3] = Block("#   #   #   #   ");
    
    //O piece
    blocks[1][0] = Block("        ##  ##  ");
    blocks[1][1] = Block("        ##  ##  ");
    blocks[1][2] = Block("        ##  ##  ");
    blocks[1][3] = Block("        ##  ##  ");
    
    //J piece
    blocks[2][0] = Block("        ###   # ");
    blocks[2][1] = Block("     #   #  ##  ");
    blocks[2][2] = Block("        #   ### ");
    blocks[2][3] = Block("    ##  #   #   ");
    
    //L piece
    blocks[3][0] = Block("        ### #   ");
    blocks[3][1] = Block("    ##   #   #  ");
    blocks[3][2] = Block("          # ### ");
    blocks[3][3] = Block("    #   #   ##  ");

    //T piece
    blocks[4][0] = Block("        ###  #  ");
    blocks[4][1] = Block("     #  ##   #  ");
    blocks[4][2] = Block("         #  ### ");
    blocks[4][3] = Block("    #   ##  #   ");
    
    //S piece
    blocks[5][0] = Block("         ## ##  ");
    blocks[5][1] = Block("    #   ##   #  ");
    blocks[5][2] = Block("         ## ##  ");
    blocks[5][3] = Block("    #   ##   #  ");
    
    //Z piece
    blocks[6][0] = Block("        ##   ## ");
    blocks[6][1] = Block("     #  ##  #   ");
    blocks[6][2] = Block("        ##   ## ");
    blocks[6][3] = Block("     #  ##  #   ");
    
    current_block_number = rand()%7;
    next_block_number = rand()%7;
}

void Gamestate::print()
{
    std::cout << "Current block: " << std::endl;
    blocks[current_block_number][0].print();
    std::cout << "Next block: " << std::endl;
    blocks[next_block_number][0].print();
    std::cout << "Lines: " << lines << std::endl;
    for(int row = 0;row<25;row++)
    {
        std::cout << "|";
        for(int col = 0;col<10;col++)
        {
            std::cout << board[row][col];
        }
        std::cout << "|" << std::endl;
    }
}

bool Gamestate::is_valid(Move move)
{
    bool res = true;
    Block current_block = blocks[current_block_number][move.rotation];
    for(int block_row = 0;block_row<4;block_row++)
    {
        for(int block_col = 0;block_col<4;block_col++)
        {
            if(current_block.cell[block_row][block_col] == '#')
            {
                int drop_col = move.column + block_col;
                if(not (0<= drop_col and drop_col < 10))
                {
                    res = false;
                }
            }
        }
    }
    return res;
}

std::vector<Move> Gamestate::get_valid_moves()
{
    std::vector<Move> res;
    for(int rotate = 0;rotate < 4;rotate++)
    {
        for(int column = 0;column < 10;column++)
        {
            Move test_move = Move(rotate,column);
            if(is_valid(test_move))
            {
                res.push_back(test_move);
            }
        }
    }
    return res;
}

void Gamestate::make_move(Move move)
{
    if(is_valid(move) == false or game_over == true)
    {
        std::cout << "Move is not allowed" << std::endl;
        return;
    }
    
    //drop starting at 0 to max 24 rows
    int row_offset = 0;
    bool can_drop = true;
    Block dropping_block = blocks[current_block_number][move.rotation];
    while(can_drop)
    {
        int test_row_offset = row_offset + 1;
        //test if row can be dropped
        for(int block_row = 0;block_row < 4;block_row++)
        {
            for(int block_col = 0;block_col < 4;block_col++)
            {
                if(dropping_block.cell[block_row][block_col] == '#')
                {
                    //test that it's on the board
                    if(block_row + test_row_offset < 25)
                    {
                        if(board[block_row + test_row_offset][block_col + move.column] == '#')
                        {
                            can_drop = false;
                        }
                    }
                    else
                    {
                        //cannot drop below the bottom of the board
                        can_drop = false;
                    }
                }
            }
        }
        
        if(can_drop)
        {
            row_offset++;
        }
    }
    
    //drop in the piece
    for(int block_row = 0;block_row<4;block_row++)
    {
        for(int block_col = 0;block_col < 4;block_col++)
        {
            if(dropping_block.cell[block_row][block_col] == '#')
            {
                board[row_offset+block_row][move.column + block_col] = '#';
            }
        }
    }
    
    //count and remove filled lines
    int line_buffer = 0;
    for(int row = 0;row<25;row++)
    {
        bool filled = true;
        for(int col = 0;col< 10;col++)
        {
            if(board[row][col] == ' ')
            {
                filled = false;
            }
        }
        if(filled)
        {
            line_buffer++;
            for(int back_row = row;back_row>0;back_row--)
            {
                for(int col = 0;col<10;col++)
                {
                    board[back_row][col] = board[back_row-1][col];
                }
            }
        }
    }
    add_lines(line_buffer);
    
    //update current and next pieces
    current_block_number = next_block_number;
    next_block_number = rand()%7;
    
    //check for game over condition
    for(int row = 0;row<5;row++)
    {
        for(int col = 0;col<10;col++)
        {
            if(board[row][col] == '#')
            {
                game_over = true;
            }
        }
    }
}

void Gamestate::clear_board()
{
    for(int row = 0;row<25;row++)
    {
        for(int col = 0;col<10;col++)
        {
            board[row][col] = ' ';
        }
    }
}

int Gamestate::count_filled_cells()
{
    int res = 0;
    for(int row = 0;row<25;row++)
    {
        for(int col = 0;col<10;col++)
        {
            if(board[row][col] == '#')
            {
                res++;
            }
        }
    }
    return res;
}

void Gamestate::add_lines(int added_lines)
{
    lines[added_lines]++;
    int current_level = get_current_level();
    score += (current_level + 1) * line_score[added_lines];
}

int Gamestate::get_current_level()
{
    int levels_added = std::max( (get_total_lines() - base_lines[start_level]) / 10 , 0);
    return start_level + levels_added;
}

int Gamestate::get_total_lines()
{
    int res = 0;
    for(int i = 0;i<5;i++)
    {
        res += i*lines[i];
    }
    return res;
}