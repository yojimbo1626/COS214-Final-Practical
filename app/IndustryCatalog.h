/**
 * @file IndustryCatalog.h
 * @brief Industry-specific data: the two demo workflows (donut shop and hair salon) and custom ones.
 *
 * This is the ONLY industry-specific code. Every industry runs on the same, unmodified
 * engine classes (include/ and src/). An industry only supplies:
 * - the products its workers can make (registered through the worker's Factory Method),
 * - the extra features a client may add to a product (Decorator prototypes),
 * - one business rule that is checked before a request is added,
 * - realistic test data to pre-fill the forms.
 */
#ifndef INDUSTRY_CATALOG_H
#define INDUSTRY_CATALOG_H

#include <sstream>
#include <string>
#include <vector>

/**
 * @struct Industry
 * @brief Data describing one industry scenario.
 *
 * The business rule is data, so the same check works for every industry:
 * - if ruleFeature is set: ruleFeature may not be added to ruleProduct;
 * - otherwise, if ruleMaxPerItem > 0: a work item may hold at most ruleMaxPerItem of ruleProduct.
 */
struct Industry {
    std::string id;                        ///< Short id used by the GUI, e.g. "donut" ("custom" for user-made ones).
    std::string title;                     ///< Display name, e.g. "Donut shop".
    std::string workerRole;                ///< What the worker is called, e.g. "baker".
    std::vector<std::string> products;     ///< Products a worker can make (WorkOutput prototypes).
    std::vector<std::string> features;     ///< Extras a client may add to a product (OutputFeature prototypes).
    std::string ruleText;                  ///< The industry rule, shown on the dashboard ("" = no rule).
    std::string ruleProduct;               ///< Product the rule is about.
    std::string ruleFeature;               ///< Extra that may not go on ruleProduct ("" = use ruleMaxPerItem).
    int ruleMaxPerItem = 0;                ///< Most ruleProduct requests per work item (0 = no limit).
    std::string defaultWorkflowName;       ///< Test data: workflow name.
    std::string defaultItemName;           ///< Test data: work item name.
    std::string defaultItemDescription;    ///< Test data: work item description.
    std::string defaultClient;             ///< Test data: client name.
    std::string defaultWorker;             ///< Test data: worker name.
};

/**
 * @brief Returns the two demo industries.
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
    donut.ruleText = "Extra icing cannot go on a Glazed donut: it is already iced.";
    donut.ruleProduct = "Glazed donut";
    donut.ruleFeature = "Extra icing";
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
    salon.ruleText = "Only one Hair colour per appointment: the colour needs the full processing time.";
    salon.ruleProduct = "Hair colour";
    salon.ruleMaxPerItem = 1;
    salon.defaultWorkflowName = "Wedding bookings";
    salon.defaultItemName = "Bridal appointment";
    salon.defaultItemDescription = "Cut, colour and styling before the wedding";
    salon.defaultClient = "Lerato Dlamini";
    salon.defaultWorker = "Ayesha";
    list.push_back(salon);

    return list;
}

/**
 * @brief Splits a comma-separated list, trimming spaces and dropping empty entries.
 * @param text e.g. "Logo, Poster ,, Flyer".
 * @return e.g. {"Logo", "Poster", "Flyer"}.
 */
inline std::vector<std::string> splitCommaList(const std::string& text) {
    std::vector<std::string> out;
    std::istringstream in(text);
    std::string part;
    while (std::getline(in, part, ',')) {
        size_t a = part.find_first_not_of(" \t");
        size_t b = part.find_last_not_of(" \t");
        if (a == std::string::npos) continue;
        out.push_back(part.substr(a, b - a + 1));
    }
    return out;
}

/**
 * @brief Builds a user-made workflow from what the user typed on the welcome page.
 * @param name The workflow name, e.g. "Print shop".
 * @param products Comma-separated products, e.g. "Poster, Flyer". Defaults to one "Standard order".
 * @param features Comma-separated extras, e.g. "Lamination". May be empty.
 * @return An industry with no business rule and no test data.
 */
inline Industry customIndustry(const std::string& name, const std::string& products, const std::string& features) {
    Industry custom;
    custom.id = "custom";
    custom.title = name;
    custom.workerRole = "worker";
    custom.products = splitCommaList(products);
    if (custom.products.empty()) custom.products.push_back("Standard order");
    custom.features = splitCommaList(features);
    custom.defaultWorkflowName = name;
    return custom;
}

/**
 * @brief Checks the industry's business rule before a request is added.
 * @param industry The industry whose rule applies.
 * @param output The output being requested, as the registry names it ("Output #2: Hair colour").
 * @param feature The extra being added to it, or "".
 * @param requests The work item's current requests.
 * @return "" if the request is allowed, otherwise why it is refused.
 */
inline std::string industryRuleViolation(const Industry& industry, const std::string& output,
                                         const std::string& feature, const std::vector<std::string>& requests) {
    if (industry.ruleProduct.empty()) return "";
    // Only the base product counts, not an extra already decorating it.
    const std::string needle = ": " + industry.ruleProduct;
    std::string base = output.substr(0, output.find(" + "));
    if (base.find(needle) == std::string::npos) return "";

    if (!industry.ruleFeature.empty()) {
        bool alreadyThere = output.find(": " + industry.ruleFeature) != std::string::npos;
        if (feature == industry.ruleFeature || alreadyThere) return industry.ruleText;
        return "";
    }
    if (industry.ruleMaxPerItem > 0) {
        int count = 0;
        for (size_t i = 0; i < requests.size(); ++i) {
            std::string b = requests[i].substr(0, requests[i].find(" + "));
            if (b.find(needle) != std::string::npos) ++count;
        }
        if (count >= industry.ruleMaxPerItem) return industry.ruleText;
    }
    return "";
}

#endif
