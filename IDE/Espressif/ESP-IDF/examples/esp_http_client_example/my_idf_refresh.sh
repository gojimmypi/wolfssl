#~/bin/bash

set -e
git fetch
git pull
git fetch upstream master
git pull  upstream master

git submodule update --init --recursive


git submodule foreach '
  # Fetch the remote branches
  git fetch

  # Try to determine the current branch
  branch=$(git symbolic-ref --short -q HEAD || git rev-parse --abbrev-ref HEAD)

  # If branch is detached, set default branch (e.g., main or master)
  if [ "$branch" = "HEAD" ]; then
    if git rev-parse --verify origin/main > /dev/null 2>&1; then
      branch="main"
    elif git rev-parse --verify origin/master > /dev/null 2>&1; then
      branch="master"
    else
      branch="HEAD"
    fi
  fi

  # Check if the branch is main or master and pull accordingly
  if [ "$branch" = "main" ] || [ "$branch" = "origin/main" ]; then
    git checkout main || git checkout -b main origin/main
    git pull origin main
  elif [ "$branch" = "master" ] || [ "$branch" = "origin/master" ]; then
    git checkout master || git checkout -b master origin/master
    git pull origin master
  else
    echo "Unknown branch or detached HEAD in submodule: $branch"
  fi
'

git submodule update --recursive --remote
