#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "component/AffineTransform.hpp"

#include <rete-reasoner/RuleParser.hpp>
#include <rete-rdf/Triple.hpp>

BOOST_AUTO_TEST_SUITE(AffineTransformTest)

    bool containsTriple(const std::vector<rete::WME::Ptr>& wmes,
                        rete::Triple triple)
    {
        for (auto wme : wmes)
        {
            auto t = std::dynamic_pointer_cast<rete::Triple>(wme);
            if (t)
            {
                if (*t == triple) return true;
            }
        }

        return false;
    }

    BOOST_AUTO_TEST_CASE(affine_create)
    {

        sempr::Core core;
        core.loadPlugins("src");
        core.loadPlugins("../src");

        core.addRules("[EC<Transform>(?e ?c), tf:create(?tf 1. 2. 3. 4. 5. 6. 7.) -> (<c> <d> <e>)]");

        auto entity = sempr::Entity::create();
        auto tf = std::make_shared<sempr::AffineTransform>();
        entity->addComponent(tf);

        core.addEntity(entity);
        core.reasoner().performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        // there should be 2: ECWME for the entity-component relation, and
        // the (<c> <d> <e>) triple.
        BOOST_CHECK(wmes.size() == 2);

        // TODO: No way to infer a AffineTransform yet, hence difficult to check here.
    }

    BOOST_AUTO_TEST_CASE(affine_get)
    {
        sempr::Core core;
        core.loadPlugins("src");
        core.loadPlugins("../src");


        // 1. get all params and add them as triples, in order to check the
        //    inferred values manually afterwards
        // 2. reduced usage of tf:get to only get x and y, not checking values
        core.addRules(
            "[EC<Transform>(?e ?tf), tf:get(?tf ?x ?y ?z ?qx ?qy ?qz ?qw)"
            "-> (<x> <value> ?x), (<y> <value> ?y), (<z> <value> ?z),"
            "   (<qx> <value> ?qx), (<qy> <value> ?qy), (<qz> <value> ?qz),"
            "   (<qw> <value> ?qw)]\n"
            "[EC<Transform>(?e ?tf), tf:get(?tf ?x ?y) -> (<foo> <bar> <baz>)]"
        );

        auto entity = sempr::Entity::create();
        auto tf = Eigen::Affine3d::Identity();
        tf.translation() = Eigen::Vector3d(1, 2, 3);
        tf.linear() = Eigen::Quaterniond(7, 4, 5, 6).toRotationMatrix();
        auto c = std::make_shared<sempr::AffineTransform>(tf);
        entity->addComponent(c);

        core.addEntity(entity);
        core.reasoner().performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        // check if the triples contain whats in the Affine3d.
        BOOST_CHECK(containsTriple(wmes, {"<x>", "<value>", std::to_string(tf.translation().x())}));
        BOOST_CHECK(containsTriple(wmes, {"<y>", "<value>", std::to_string(tf.translation().y())}));
        BOOST_CHECK(containsTriple(wmes, {"<z>", "<value>", std::to_string(tf.translation().z())}));
        BOOST_CHECK(containsTriple(wmes, {"<qx>", "<value>", std::to_string(Eigen::Quaterniond(tf.rotation()).x())}));
        BOOST_CHECK(containsTriple(wmes, {"<qy>", "<value>", std::to_string(Eigen::Quaterniond(tf.rotation()).y())}));
        BOOST_CHECK(containsTriple(wmes, {"<qz>", "<value>", std::to_string(Eigen::Quaterniond(tf.rotation()).z())}));
        BOOST_CHECK(containsTriple(wmes, {"<qw>", "<value>", std::to_string(Eigen::Quaterniond(tf.rotation()).w())}));

        BOOST_CHECK(containsTriple(wmes, {"<foo>", "<bar>", "<baz>"}));
    }


    BOOST_AUTO_TEST_CASE(affine_mul)
    {
        sempr::Core core;
        core.loadPlugins("src");
        core.loadPlugins("../src");

        core.addRules(
            "[EC<Transform>(?e ?left \"left\"),"
            " EC<Transform>(?e ?right \"right\"),"
            " tf:mul(?result ?left ?right)"
            " -> "
            " (<foo> <bar> <baz>)]"
        );

        auto entity = sempr::Entity::create();

        auto tfleft = Eigen::Affine3d::Identity();
        tfleft.translation() = Eigen::Vector3d(1, 2, 3);
        tfleft.linear() = Eigen::Quaterniond(7, 4, 5, 6).toRotationMatrix();

        auto tfright = Eigen::Affine3d::Identity();
        tfright.translation() = Eigen::Vector3d(0.5, 0.6, 0.7);
        tfright.linear() = Eigen::Quaterniond(2, 8, 4, 6).toRotationMatrix();


        auto left = std::make_shared<sempr::AffineTransform>(tfleft);
        entity->addComponent(left, "left");

        auto right = std::make_shared<sempr::AffineTransform>(tfright);
        entity->addComponent(right, "right");

        core.addEntity(entity);
        core.reasoner().performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        BOOST_CHECK(containsTriple(wmes, {"<foo>", "<bar>", "<baz>"}));
        // TODO need an EC(?e ?c ?tag) effect to check this correctly!
        // -> EC(?e ?result "result")
        // - search for ECWME with result
        // - compare tf with directly computed one
        //
        std::ofstream("affine_mul.dot") << core.reasoner().net().toDot();
    }

    BOOST_AUTO_TEST_CASE(affine_inv)
    {
        sempr::Core core;
        core.loadPlugins("src");
        core.loadPlugins("../src");

        core.addRules(
            "[EC<Transform>(?e ?tf),"
            " tf:inv(?result ?tf)"
            " -> "
            " (<foo> <bar> <baz>)]"
        );

        auto entity = sempr::Entity::create();

        auto tfin = Eigen::Affine3d::Identity();
        tfin.translation() = Eigen::Vector3d(1, 2, 3);
        tfin.linear() = Eigen::Quaterniond(7, 4, 5, 6).toRotationMatrix();

        auto input = std::make_shared<sempr::AffineTransform>(tfin);
        entity->addComponent(input);

        core.addEntity(entity);
        core.reasoner().performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        BOOST_CHECK(containsTriple(wmes, {"<foo>", "<bar>", "<baz>"})); // TODO: same as in affine_mul
        std::ofstream("affine_inv.dot") << core.reasoner().net().toDot();
    }


    BOOST_AUTO_TEST_CASE(affine_combination)
    {
        sempr::Core core;
        core.loadPlugins("src");
        core.loadPlugins("../src");


        core.addRules(
            "[EC<Transform>(?e ?tf), tf:inv(?inv ?tf), tf:mul(?id ?inv ?tf) -> (<foo> <bar> <baz>)]"
        );

        auto entity = sempr::Entity::create();
        auto aff = Eigen::Affine3d::Identity();
        aff.linear() = Eigen::Quaterniond(1, 2, 3, 4).toRotationMatrix();
        aff.translation() = Eigen::Vector3d(1, 2, 3);
        auto tf = std::make_shared<sempr::AffineTransform>(aff);
        entity->addComponent(tf);

        core.addEntity(entity);
        core.reasoner().performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        // there should be 2: ECWME for the entity-component relation, and
        // the (<foo> <bar> <baz>) triple.
        BOOST_CHECK(wmes.size() == 2);

        // TODO: No way to infer a AffineTransform yet, hence difficult to check here.

        std::ofstream("affine_comb.dot") << core.reasoner().net().toDot();
    }



BOOST_AUTO_TEST_SUITE_END()
