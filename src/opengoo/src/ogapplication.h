#ifndef OGAPPLICATION_H
#define OGAPPLICATION_H

class OGApplication
{
public:   
    int run(int argc, char** argv);

private:
    bool initialize(int argc, char** argv);
    void clear();
};

#endif // OGAPPLICATION_H
