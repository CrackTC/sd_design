cmake_minimum_required(VERSION 3.24)
project(design)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-g")

add_executable(design utils.c data/linkedList.c data/journal.c data/item.c data/amount.c data/inventory.c data/basicDiscount.c data/customer.c data/lossEntry.c data/operation.c data/permission.c data/order.c data/profit.c data/refundEntry.c data/serialization.c data/staff.c data/table.c data/time.c gui/detail/itemDetail.c gui/edit/inventoryEdit.c gui/edit/discountEdit.c gui/page/customerPage.c gui/edit/customerEdit.c gui/page/itemPage.c gui/detail/customerDetail.c gui/page/inventoryPage.c gui/page/discountPage.c gui/detail/inventoryDetail.c gui/detail/discountDetail.c gui/edit/itemEdit.c gui/detail/journalDetail.c gui/page/journalPage.c gui/layout.c gui/loginWindow.c gui/detail/lossDetail.c gui/edit/lossEdit.c gui/page/lossPage.c gui/main.c gui/mainWindow.c gui/detail/orderDetail.c gui/edit/orderEdit.c gui/detail/profitDetail.c gui/page/orderPage.c gui/page/profitPage.c gui/detail/refundDetail.c gui/edit/refundEdit.c gui/page/refundPage.c gui/detail/staffDetail.c gui/edit/staffEdit.c gui/page/staffPage.c gui/page/welcomePage.c gui/tableLayout.c services/inventoryService.c services/customerService.c services/journalService.c services/judgeService.c services/saleService.c services/staffService.c services/statisticService.c gui/glad.c gui/resultDialog.c)
target_link_libraries(design /usr/lib/libm.so /usr/lib/libGLEW.so /usr/lib/libGL.so /usr/lib/libglfw.so)
target_include_directories(design PUBLIC include/)