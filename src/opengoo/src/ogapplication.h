#pragma once

class OGApplication
{
public:   
    int run(int argc, char** argv);

private:
    bool initialize(int argc, char** argv);
    void clear();
};
