/**
 * @file IndustryCatalog.h
 * @brief Industry-specific data for the two TIVIDY scenarios (donut shop and hair salon).
 *
 * This is the ONLY industry-specific code. Both industries run on the same, unmodified
 * engine classes (include/ and src/). An industry only supplies:
 * - the products its workers can make (registered through the worker's Factory Method),
 * - the extra features a client may add to a product (Decorator prototypes),
 * - realistic test data to pre-fill the forms.
 */
#ifndef INDUSTRY_CATALOG_H
#define INDUSTRY_CATALOG_H

#include <string>
#include <vector>

/**
 * @struct Industry
 * @brief Data describing one industry scenario.
 */
struct Industry {
    std::string id;                        ///< Short id used by the GUI, e.g. "donut".
    std::string title;                     ///< Display name, e.g. "Donut shop".
    std::string workerRole;                ///< What the worker is called, e.g. "baker".
    std::vector<std::string> products;     ///< Products a worker can make (WorkOutput prototypes).
    std::vector<std::string> features;     ///< Extras a client may add to a product (OutputFeature prototypes).
    std::string defaultWorkflowName;       ///< Test data: workflow name.
    std::string defaultItemName;           ///< Test data: work item name.
    std::string defaultItemDescription;    ///< Test data: work item description.
    std::string defaultClient;             ///< Test data: client name.
    std::string defaultWorker;             ///< Test data: worker name.
};

/**
 * @brief Returns the two industries TIVIDY supports.
 * @return Donut shop and hair salon, in that order.
 */
inline std::vector<Industry> industryCatalog() {
    std::vector<Industry> list;

    Industry donut;
    donut.id = "donut";
    donut.title = "Donut shop";
    donut.workerRole = "baker";
    donut.products.push_back("Glazed donut");
    donut.products.push_back("Chocolate donut");
    donut.products.push_back("Jam donut");
    donut.products.push_back("Boston cream donut");
    donut.products.push_back("Cinnamon sugar donut");
    donut.features.push_back("Sprinkles");
    donut.features.push_back("Extra icing");
    donut.features.push_back("Gift box");
    donut.defaultWorkflowName = "Saturday orders";
    donut.defaultItemName = "Party order";
    donut.defaultItemDescription = "Three dozen donuts for a birthday party";
    donut.defaultClient = "Thandi Mokoena";
    donut.defaultWorker = "Sipho";
    list.push_back(donut);

    Industry salon;
    salon.id = "salon";
    salon.title = "Hair salon";
    salon.workerRole = "stylist";
    salon.products.push_back("Haircut");
    salon.products.push_back("Hair colour");
    salon.products.push_back("Blow-dry");
    salon.products.push_back("Braids");
    salon.products.push_back("Beard trim");
    salon.features.push_back("Deep-conditioning treatment");
    salon.features.push_back("Scalp massage");
    salon.features.push_back("Styling product");
    salon.defaultWorkflowName = "Wedding bookings";
    salon.defaultItemName = "Bridal appointment";
    salon.defaultItemDescription = "Cut, colour and styling before the wedding";
    salon.defaultClient = "Lerato Dlamini";
    salon.defaultWorker = "Ayesha";
    list.push_back(salon);

    return list;
}

#endif
