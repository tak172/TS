#pragma once

#include "../Fund/ArchivistGateway.h"
#include "../helpful/Dictum.h"

class UtGateway : public ArchivistGateway
{
public:
    typedef std::pair< std::string, F2A_BBX_OPTION > DocAndOpt;
    typedef std::vector< DocAndOpt > vecDocAndOpt;

    UtGateway();
    ~UtGateway();
    
    /** @brief Read black box record, reference or not, 
               with lacuna specified, returns true if no error occured */
    virtual bool read(const std::vector<char>& u8caption, const std::vector<char>& u8data, const F2A_BBX_OPTION& opt) override;

    /** @brief Returns gateway description */
    virtual std::wstring description() const override;

    void PopReadedMessages( vecDocAndOpt& vec );
    void PopSeekingResults( vecDocAndOpt& vec );
    void PopAll();
    bool isOverstock() const override;
    void setOverstock(bool val);
    bool needNotify() const override;
    bool needAsync() const override;
    void predictAhead( time_t /*next_moment*/ ) override {};
    
private:
    /** @brief Guard readed field */
    boost::mutex    mtx;
    /** @brief Vector readed field */
    vecDocAndOpt    dao;

    /** @brief признак затоваривания */
    bool overstock;
	/** @brief Извлечение элементов с указанным состояние fin */
	void PopWithFlag( bool fin_state, vecDocAndOpt& vec );
};
inline bool UtGateway::isOverstock() const
{
    return overstock;
}
inline void UtGateway::setOverstock(bool val)
{
    overstock = val;
}
inline bool UtGateway::needNotify() const
{
    return true;
}
inline bool UtGateway::needAsync() const
{
    return false;
}
