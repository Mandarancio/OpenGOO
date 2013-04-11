#ifndef OG_IPIPE_H
#define OG_IPIPE_H

class QPainter;

class OGIPipe
{
        virtual void _Close() = 0;
        virtual void _Open() = 0;
        virtual void _Painter(QPainter* painter) = 0;

    public:
        OGIPipe() {}
        virtual ~OGIPipe() {}

        void Close() { _Close(); }  // Close the pipe
        void Open() { _Open(); } // Open the pipe
        void Painter(QPainter* painter) { _Painter(painter); } // Draw the pipe
};

#endif // OG_IPIPE_H
