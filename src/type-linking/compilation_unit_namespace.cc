#include "compilation_unit_namespace.h"
#include "environment-builder/symboltable.h"
#include "exceptions/semanticerror.h"
#include "exceptions/exceptions.h"
#include "utillities/util.h"
#include <optional>
#include <functional>

CompilationUnitNamespace::CompilationUnitNamespace(
    PackageDeclarationObject *current_package,
    PackageDeclarationObject *default_package,
    TypeDeclaration declared_type,
    std::vector<TypeDeclaration> single_imports,
    std::vector<PackageDeclarationObject*> star_imports
) : 
    current_package{current_package},
    default_package{default_package},
    declared_type{declared_type},
    single_imports{single_imports},
    star_imports{star_imports}
{}

TypeDeclaration resolveCandidates(std::vector<TypeDeclaration>& valid_candidates, std::string identifier) {
    util::removeDuplicates(valid_candidates);
    if (valid_candidates.size() == 0) {
        THROW_TypeLinkerError("Type declaration for " + identifier + " not found");
    } else if (valid_candidates.size() > 1) {
        std::string error_message = "Type '" + identifier + "' is ambigious. Possible candidates are ";
        for (auto candidate : valid_candidates) {
            std::visit([&](auto type){ error_message += "'" + type->identifier + "'"; }, candidate);
            if (candidate != valid_candidates.back()) {
                error_message += ", ";
            }
        }
        THROW_TypeLinkerError(error_message);
    }
    return valid_candidates.back();
}

TypeDeclaration CompilationUnitNamespace::lookupQualifiedType(QualifiedIdentifier &qualified_identifier) {
    /*
        * Typelinking:
        * - Qualified names of the form Q.id are handled by these rules: 
        *   1. Q must be a package name in the environment.
        *   2. No prefix of Q, including Q, can be a type name.
        *   3. id must be exactly one type that is a member of Q.
    */
    QualifiedIdentifier package_qid = qualified_identifier.getQualifiedIdentifierWithoutLast();
    std::string canoncial_name = qualified_identifier.identifiers.back().name;

    if (package_qid.identifiers.empty()) {
        auto valid_candidates = lookupSimpleType(canoncial_name);
        return resolveCandidates(valid_candidates, canoncial_name);
    }

    // Check each import-on-demand package in compilation units namespace
    auto getValidCandidates = [&](QualifiedIdentifier &package_qid, std::string canoncial_name){
        std::vector<TypeDeclaration> valid_candidates;
        
        auto all_packages = star_imports;
        all_packages.push_back(default_package);
        all_packages.push_back(current_package);
        util::removeDuplicates(all_packages);
        
        for (auto package : all_packages) {
            auto possible_package = package->tryFindPackageInPackage(package_qid);
            if (possible_package) {
                if (auto possible_type = (*possible_package)->tryFindTypeInPackage(canoncial_name)) {
                    valid_candidates.push_back(*possible_type);
                }
            }
        }

        return valid_candidates;
    };

    // Verify 1.
    auto valid_candidates = getValidCandidates(package_qid, canoncial_name);

    // Verify 2.
    std::string conflicting_prefix;
    // Return true if any non-strict prefix of qid resolves to a type in the environment.
    std::function<bool(QualifiedIdentifier&)> somePrefixIsType = [&](QualifiedIdentifier &qid){
        auto prefix = qid.getQualifiedIdentifierWithoutLast();
        std::string canoncial_name = qid.identifiers.back().name;

        if (prefix.identifiers.empty()) {
            // This is a simple type
            conflicting_prefix = canoncial_name;
            return !lookupSimpleType(canoncial_name).empty();
        } else if (!getValidCandidates(prefix, canoncial_name).empty()) {
            conflicting_prefix = qid.getQualifiedName();
            return true;
        }
        return somePrefixIsType(prefix);
    };
    if (somePrefixIsType(package_qid)) {
        auto stringified_qid = qualified_identifier.getQualifiedName();
        THROW_TypeLinkerError(
            "A strict prefix, " + conflicting_prefix 
                + ", of the fully qualified type " + stringified_qid + " resolves to a type."
        );
    }

    // Verify 3.
    return resolveCandidates(valid_candidates, canoncial_name);
}

std::vector<TypeDeclaration> CompilationUnitNamespace::lookupSimpleType(std::string &identifier) {
    /*
        * Typelinking:
        * - Unqualified names are handled by these rules: 
        *   1. try the class or interface of the compilation unit, or any single-type-import (A.B.C.D) 
        *   2. try the same package, across all compilation units
        *   3. try any import-on-demand package (A.B.C.*), including java.lang.* 
    */
    std::vector<TypeDeclaration> valid_candidates;

    // 1a. Try the type declared in this compilation unit, if it exists
    std::visit([&](auto class_or_interface) {
        if (class_or_interface && class_or_interface->identifier == identifier) {
            valid_candidates.push_back(class_or_interface);
        }
    }, declared_type);

    // 1b. Look up in list of all single type imports
    for (auto type_dec : single_imports) {
        std::visit([&](auto class_or_int_dec) {
            if (class_or_int_dec->identifier == identifier) {
                valid_candidates.push_back(type_dec);
            }
        }, type_dec);
    }

    if (valid_candidates.size() > 0) {
        return valid_candidates;
    }

    // 2. Look up in current package
    if (auto possible_type = current_package->tryFindTypeInPackage(identifier)) {
        valid_candidates.push_back(*possible_type);
    }

    if(valid_candidates.size() > 0) {
        return valid_candidates;
    }

    // 3. Look up in imported packages
    for (auto package_dec : star_imports) {
        auto possible_classes = package_dec->classes->lookupSymbol(identifier);
        auto possible_interfaces = package_dec->interfaces->lookupSymbol(identifier);

        if (possible_classes) {
            for (auto &possible_class : *possible_classes) {
                TypeDeclaration found_type = &std::get<ClassDeclarationObject>(possible_class);
                valid_candidates.push_back(found_type);
            }
        }
        if (possible_interfaces) {
            for (auto &possible_interface : *possible_interfaces) {
                TypeDeclaration found_type = &std::get<InterfaceDeclarationObject>(possible_interface);
                valid_candidates.push_back(found_type);
            }
        }
    }

    return valid_candidates;
}