/** Copyright 2018 RWTH Aachen University. All rights reserved.
 *
 *  Licensed under the RWTH ASR License (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.hltpr.rwth-aachen.de/rwth-asr/rwth-asr-license.html
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <Test/UnitTest.hh>
#include <Test/File.hh>
#include <Core/Directory.hh>

TEST(Test, File, CreateDirectory)
{
    ::Test::Directory *dir = new ::Test::Directory;
    std::string path = dir->path();
    EXPECT_TRUE(Core::isDirectory(path));
    delete dir;
    EXPECT_FALSE(Core::isDirectory(path));
}

TEST(Test, File, CreateFile)
{
    ::Test::Directory *dir = new ::Test::Directory;
    std::string filename = ::Test::File(*dir, "abc.de").path();
    EXPECT_EQ(Core::directoryName(filename), dir->path());
}
