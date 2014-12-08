/*
 * Copyright (C) 2014  Xiao-Long Chen <chenxiaolong@cxl.epac.to>
 *
 * This file is part of MultiBootPatcher
 *
 * MultiBootPatcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MultiBootPatcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MultiBootPatcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ramdiskpatchers/galaxy/galaxyramdiskpatcher.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>

#include "private/regex.h"
#include "ramdiskpatchers/common/coreramdiskpatcher.h"


/*! \cond INTERNAL */
class GalaxyRamdiskPatcher::Impl
{
public:
    const PatcherConfig *pc;
    const FileInfo *info;
    CpioFile *cpio;

    std::string version;

    PatcherError error;
};
/*! \endcond */


/*! \brief Android Jelly Bean */
const std::string GalaxyRamdiskPatcher::JellyBean("jb");
/*! \brief Android Kit Kat */
const std::string GalaxyRamdiskPatcher::KitKat("kk");

static const std::string InitRc("init.rc");
static const std::string InitTargetRc("init.target.rc");
static const std::string UeventdRc("ueventd.rc");
static const std::string UeventdQcomRc("ueventd.qcom.rc");
static const std::string Msm8960LpmRc("MSM8960_lpm.rc");


/*!
    \class GalaxyRamdiskPatcher
    \brief Handles common ramdisk patching operations for Samsung Galaxy devices.
 */

/*!
    Constructs a ramdisk patcher associated with the ramdisk of a particular
    kernel image.

    The \a cpio is a pointer to a CpioFile on which all of the operations will
    be applied. If more than one ramdisk needs to be patched, create a new
    instance for each one. The \a version parameter specifies which Android
    version the ramdisk is from (GalaxyRamdiskPatcher::JellyBean,
    GalaxyRamdiskPatcher::KitKat, etc.).
 */
GalaxyRamdiskPatcher::GalaxyRamdiskPatcher(const PatcherConfig * const pc,
                                           const FileInfo * const info,
                                           CpioFile * const cpio,
                                           const std::string &version) :
    m_impl(new Impl())
{
    m_impl->pc = pc;
    m_impl->info = info;
    m_impl->cpio = cpio;
    m_impl->version = version;
}

GalaxyRamdiskPatcher::~GalaxyRamdiskPatcher()
{
}

PatcherError GalaxyRamdiskPatcher::error() const
{
    return m_impl->error;
}

std::string GalaxyRamdiskPatcher::id() const
{
    return std::string();
}

bool GalaxyRamdiskPatcher::patchRamdisk()
{
    return false;
}

/*!
    \brief Patches MSM8960_lpm.rc in TouchWiz and Google Edition ramdisks

    \note The method does not do anything for Kit Kat ramdisks.

    This method comments out the line that mounts \c /cache in
    \c MSM8960_lpm.rc.

    \return Succeeded or not
 */
bool GalaxyRamdiskPatcher::getwModifyMsm8960LpmRc()
{
    // This file does not exist on Kit Kat ramdisks
    if (m_impl->version == KitKat) {
        return true;
    }

    auto contents = m_impl->cpio->contents(Msm8960LpmRc);
    if (contents.empty()) {
        m_impl->error = PatcherError::createCpioError(
                MBP::ErrorCode::CpioFileNotExistError, Msm8960LpmRc);
        return false;
    }

    std::string strContents(contents.begin(), contents.end());
    std::vector<std::string> lines;
    boost::split(lines, strContents, boost::is_any_of("\n"));

    for (auto it = lines.begin(); it != lines.end(); ++it) {
        if (MBP_regex_search(*it, MBP_regex("^\\s+mount.*/cache.*$"))) {
            it->insert(it->begin(), '#');
        }
    }

    strContents = boost::join(lines, "\n");
    contents.assign(strContents.begin(), strContents.end());
    m_impl->cpio->setContents(Msm8960LpmRc, std::move(contents));

    return true;
}
