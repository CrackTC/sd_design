#ifndef CONFIG_H
#define CONFIG_H

static const char *const fontRelativePath = "resources/MiSans-Medium.ttf";

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#define PATH_SEPARATOR_STRING "\\"
#else
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#endif

extern const char *executablePath;

extern struct nk_font *fontLarge;
extern struct nk_font *fontMedium;
extern struct nk_font *fontSmall;

#define SECTION_COUNT 11

enum SectionIndexes
{
    WELCOME_INDEX,
    ITEM_INDEX,
    INVENTORY_INDEX,
    LOSS_INDEX,
    ORDER_INDEX,
    REFUND_INDEX,
    DISCOUNT_INDEX,
    CUSTOMER_INDEX,
    STAFF_INDEX,
    JOURNAL_INDEX,
    PROFIT_INDEX,
};

#define WELCOME_NAME ""
#define ITEM_NAME "商品"
#define INVENTORY_NAME "库存"
#define LOSS_NAME "货损"
#define ORDER_NAME "订单"
#define REFUND_NAME "退货"
#define DISCOUNT_NAME "折扣"
#define CUSTOMER_NAME "客户"
#define STAFF_NAME "员工"
#define JOURNAL_NAME "日志"
#define PROFIT_NAME "统计"

#endif
