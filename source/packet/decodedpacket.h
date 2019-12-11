#ifndef DECODEDPACKET_H
#define DECODEDPACKET_H
#include <QByteArray>
#include <QtGlobal>

class DecodedPacket
{
public:
    enum DecodeResult{
        Success,
        Failure,
        Unknown
    };
private:
    int errorLength_priv;
    int8_t errorArray_priv[2] = {0};
    QByteArray decodedPacket_priv;
    DecodeResult result_priv;
public:    
    DecodedPacket(DecodeResult result_priv, int errorLength_priv, int8_t errorArray_priv[], QByteArray decodedPacket_priv);
    int getErrorLength() const;
    const int8_t* getErrorArray() const;
    DecodeResult getResult() const;
    QByteArray getDecodedPacket() const;

};

#endif // DECODEDPACKET_H
