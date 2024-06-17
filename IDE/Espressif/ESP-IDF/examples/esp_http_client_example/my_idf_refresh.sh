#~/bin/bash

set -e
git fetch
git pull
git fetch upstream master
git pull  upstream master

git submodule update --init --recursive


git submodule foreach '
  git fetch origin

  # Determine the default branch (main or master)
  if git show-ref --verify --quiet refs/heads/main; then
    default_branch="main"
  elif git show-ref --verify --quiet refs/heads/master; then
    default_branch="master"
  else
    echo "No main or master branch found in submodule."
    exit 1
  fi

  # Check out the default branch and reset to the latest commit
  git checkout $default_branch
  git reset --hard origin/$default_branch
'

git submodule update --recursive --remote
