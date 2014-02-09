#ifndef __CARD_H__
#define __CARD_H__

//Card definition
typedef enum
{
  card_empty = 0x00,

  value_as    = 0x01,
  value_two   = 0x02,
  value_three = 0x03,
  value_four  = 0x04,
  value_five  = 0x05,
  value_six   = 0x06,
  value_seven = 0x07,
  value_eight = 0x08,
  value_nine  = 0x09,
  value_ten   = 0x0A,
  value_J     = 0x0B,
  value_Q     = 0x0C,
  value_K     = 0x0D,
  
  value_mask = 0x0F,

  color_pique  = 0x00,
  color_trefle = 0x10,
  color_heart  = 0x20,
  color_square = 0x30,

  color_mask = 0x30,
} tCard;

#define setCard(value, color) (static_cast<tCard>((value) | (color)))

#define getValue(card) static_cast<tCard>((card) & value_mask)

#define getColor(card) ((card) & color_mask)

#define getNextColor_As(card) static_cast<tCard>(getColor(card) + 0x10)
#define getNextCard(card) static_cast<tCard>((getValue(card) == value_K)? (getNextColor_As(card) | value_as): card+1)

#define NUMBER_OF_CARDS 52

#endif //__CARD_H__